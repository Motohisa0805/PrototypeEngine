#include "ScriptHotReloadManager.h"
#include "SceneManager.h"
#include "ScriptComponent.h"
#include "ScriptEditManager.h"
#include "ProjectPanel.h"

ScriptHotReloadManager::ScriptHotReloadManager()
	: mSourceDllPath("bin\\InGameProject.dll")
	, mActiveDllPath("bin\\InGameProject_Active.dll")
	, mSourcePDBPath("bin\\InGameProject.pdb")
	, mActivePDBPath("bin\\InGameProject_Active.pdb")
	, mDllIndex(0)
	, mLastLoadTime{ 0 }
	, mScriptsDirectory("Assets/")
{
}

ScriptHotReloadManager::~ScriptHotReloadManager()
{
	remove(mActiveDllPath.c_str());
	remove(mActivePDBPath.c_str());
}

bool ScriptHotReloadManager::Initialize()
{

	// 最初のDLLロード
	const string& sourcePath = mSourceDllPath;
	//初回ロードに使うコピー先のファイルパス
	string initialDllPath = mActiveDllPath;

	//初回起動時、オリジナルDLLをアクティブDLLとしてコピー
	if(!CopyFileA(sourcePath.c_str(), initialDllPath.c_str(), FALSE))
	{
		Debug::ErrorLog("File Copy Dll File",mActiveDllPath);
		return false;
	}

	mActiveDllPath = initialDllPath;

	// 2. PDBの初回コピー
	const string& sourcePDBPath = mSourcePDBPath;
	// 初回ロードに使うコピー先のファイルパス (DLLと一致させる)
	string initialPDBPath = mActivePDBPath;

	if (!CopyFileA(sourcePDBPath.c_str(), initialPDBPath.c_str(), FALSE))
	{
		Debug::ErrorLog("File Copy PDB File"); // PDB File
		return false;
	}
	mActivePDBPath = initialPDBPath;




	// 【重要】初回ロード時刻をセットし、以降の変更監視を可能にする
	mLastLoadTime = GetDllLastWriteTime(mSourceDllPath);
	return true;
}

bool ScriptHotReloadManager::LoadScripts()
{
	//1.DLLのロード
	mCurrentDll.hDll = LoadLibraryA(mActiveDllPath.c_str());
	if (!mCurrentDll.hDll)
	{
		//ロード失敗
		Debug::ErrorLog("Failed to load DLL: ", mActiveDllPath.c_str());
		return false;
	}

	//2.ファクトリー関数の取得
	mCurrentDll.CreateFunc = (CreateScriptComponentFunc)GetProcAddress(mCurrentDll.hDll, "CreateScriptComponent");

	mCurrentDll.UnregisterFunc = (UnregisterScriptComponentsFunc)GetProcAddress(mCurrentDll.hDll, "UnregisterScriptComponents");

	if (!mCurrentDll.CreateFunc)
	{
		//関数取得失敗
		FreeLibrary(mCurrentDll.hDll);
		mCurrentDll = { };
		return false;
	}

	// DLLのDllMainが呼ばれ、その中でComponentRegistrarが静的に実行され、クラスが登録されます
	mLastLoadTime = GetDllLastWriteTime(mSourceDllPath);
	return true;
}

bool ScriptHotReloadManager::ReloadInGameProject()
{
	// ----------------------------------------------------
	// 1.【準備】: 既存のスクリプトインスタンスの状態を保存
	// ----------------------------------------------------
	for (ActorObject* actor : SceneManager::GetAllActorsInCurrentScene())
	{
		//削除リストを一時的に作成
		vector<ScriptComponent*> componentToDelete;

		for (Component* comp : actor->GetComponents())
		{
			if (ScriptComponent* script = dynamic_cast<ScriptComponent*>(comp))
			{
				json compData;
				script->Serialize(compData);//現在の状態をシリアライズ
				mSavedState[actor].push_back({
					script->GetName(),
					compData
					});
			
				//削除対象を追加
				componentToDelete.push_back(script);
			}
		}

		//コンポーネントのメモリ解放
		for (ScriptComponent* script : componentToDelete)
		{
			delete script;
		}
	}

	// ----------------------------------------------------
	// 2. 【古いDLLのアンロード】
	// ----------------------------------------------------
	UnloadScripts();

	// ----------------------------------------------------
	// 3. 【新しいDLLのコンパイル】
	// ----------------------------------------------------
	string newDllPath = mActiveDllPath;
	
	//開発者がビルドしたDLLを新しい名前にコピー
	if (CopyFileA(mSourceDllPath.c_str(), newDllPath.c_str(), FALSE))
	{
		mActiveDllPath = newDllPath;//ロードするパス
	}
	else
	{
		Debug::ErrorLog("File Copy Dll File");
		return false;
	}

	// B) PDBのコピー
	// DLLと同じインデックス（1 or 2）を持つPDBパスを作成
	string newPDBPath = mActivePDBPath;

	// オリジナル(mSourcePDBPath)を新しいPDB(newPDBPath)にコピー
	if (!CopyFileA(mSourcePDBPath.c_str(), newPDBPath.c_str(), FALSE))
	{
		Debug::ErrorLog("File Copy PDB File Failed");
		return false;
	}

	// mActivePDBPathを更新 (デバッガがこのパスを参照できるように)
	mActivePDBPath = newPDBPath;




	// ----------------------------------------------------
	// 4. 【新しいDLLのロード】
	// ----------------------------------------------------
	if (!LoadScripts())
	{
		// ロード失敗
		return false;
	}

	// ----------------------------------------------------
	// 5. 【復元】: 新しいDLLのインスタンスを生成し、状態を復元
	// ----------------------------------------------------
	// 実際のロジックのイメージ
	for (const auto& pair : mSavedState)
	{
		ActorObject* actor = pair.first;
		for (const auto& savedScript : pair.second)
		{
			//新しいDLLのファクトリ関数を呼び出してインスタンスを生成
			Component* newComp = mCurrentDll.CreateFunc(
				savedScript.ClassName.c_str(),actor
			);
			//状態をデシリアライズ
			newComp->Deserialize(savedScript.Data);//新しいDLLのDeserializeを呼び出し
			//newCompをactorにアタッチ

			ScriptComponent* newScript = dynamic_cast<ScriptComponent*>(newComp);
			if (newScript)
			{
				newScript->SetStarted(true);
			}
		}
	}
	mSavedState.clear();
	return true;
}

void ScriptHotReloadManager::UnloadScripts()
{
	// 1.DLL内のクラス登録解除: DLLが持つクラス登録情報をエンジンから消去
	if (mCurrentDll.UnregisterFunc)
	{
		mCurrentDll.UnregisterFunc();
	}

	if(mCurrentDll.hDll)
	{
		// 2.DLLのアンロード: DLLをメモリから解放
		FreeLibrary(mCurrentDll.hDll);
		remove(mActiveDllPath.c_str());
		remove(mActivePDBPath.c_str());
	}
	mCurrentDll = { };
}

bool ScriptHotReloadManager::CheckForChanges()
{
	std::set<string> assetsAddedOrModified;
	std::set<string> assetsRemoved;

	//ディスクの現状をスキャンして、変更を検出
	std::set<string> currentFilesInAssets;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(mScriptsDirectory))
	{
		if (entry.is_regular_file())
		{
			string path = entry.path().lexically_normal().string();
			std::replace(path.begin(), path.end(), '\\', '/');
			//.cppと.hのみ対象
			if (path.ends_with(".cpp") || path.ends_with(".h"))
			{
				currentFilesInAssets.insert(path);

				FILETIME lastWriteTime = GetDllLastWriteTime(path);

				if (mKnownAssetTimestamps.find(path) == mKnownAssetTimestamps.end())
				{
					assetsAddedOrModified.insert(path);
					mKnownAssetTimestamps[path] = lastWriteTime;
				}
				else if (CompareFileTime(&lastWriteTime, &mKnownAssetTimestamps[path]) > 0)
				{
					assetsAddedOrModified.insert(path);
					mKnownAssetTimestamps[path] = lastWriteTime;
				}
			}
		}
	}

	//削除されたファイルの検出
	// --- 削除されたファイルを検出 ---
	for (const auto& pair : mKnownAssetTimestamps)
	{
		const std::string& knownPath = pair.first;
		if (currentFilesInAssets.find(knownPath) == currentFilesInAssets.end())
		{
			assetsRemoved.insert(knownPath);
		}
	}

	// 既知のリストから削除
	for (const auto& path : assetsRemoved)
	{
		mKnownAssetTimestamps.erase(path);
	}

	// --- 変更を処理 ---

	// 初回スキャン時は、変更を処理しない (ベースラインを作成するだけ)
	if (!mFirstScanComplete)
	{
		mFirstScanComplete = true;
		return false;
	}

	bool needsRebuild = false;
	std::set<std::string> processedStems;

	// A) 削除処理
	for (const auto& path : assetsRemoved)
	{
		// stemによるガードを外し、削除されたファイル（.cppも.hも）をすべて処理に回す
		if (ScriptEditManager::GetInstance().RemoveScriptFileToVcxProj(path, ""))
		{
			needsRebuild = true;
		}
	}

	// B) 追加・変更処理
	for (const auto& path : assetsAddedOrModified)
	{
		std::string stem = std::filesystem::path(path).stem().string();
		if (processedStems.find(stem) == processedStems.end())
		{
			ScriptEditManager::GetInstance().AddScriptFileToVcxProj(path, stem);
			processedStems.insert(stem);
			needsRebuild = true;
		}
	}
	if (needsRebuild)
	{
		Debug::Log("Needs Rebuild");
		// どのファイルが原因でリロードが走っているか表示
		for (auto& p : assetsAddedOrModified)
		{
			Debug::Log("Added/Modified: ");
			Debug::Log(p.c_str());
		}
		for (auto& p : assetsRemoved)
		{
			Debug::Log("Removed/Modified: ");
			Debug::Log(p.c_str());
		}
		return ExecuteMsbuildAndReload();
	}

	return false;
}

FILETIME ScriptHotReloadManager::GetDllLastWriteTime(const string& filePath)
{
	FILETIME lastWriteTime = { 0 };
	//ファイルを開く
	HANDLE hFile = CreateFileA(
		filePath.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		//ファイルが見つからない、またはエラー
		Debug::ErrorLog("Failed to open DLL file for time check.");
		return lastWriteTime;
	}

	//最終書き込み時刻を取得
	if (!GetFileTime(hFile, NULL, NULL, &lastWriteTime))
	{
		Debug::ErrorLog("Failed to get DLL file time.");
	}


	CloseHandle(hFile);
	return lastWriteTime;
}

int ScriptHotReloadManager::ExecuteAndWaitForProcess(const string& command)
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	//CreateProcessは書き換え可能な文字列が必要
	vector<char> commandLine(command.begin(), command.end());
	commandLine.push_back('\0');

	//CreateProcessの呼び出しを追加
	BOOL success = CreateProcessA(
		NULL,             // モジュール名
		&commandLine[0],  // コマンドライン
		NULL,             // プロセスセキュリティ属性
		NULL,             // スレッドセキュリティ属性
		FALSE,            // ハンドルの継承
		0,                // 作成フラグ
		NULL,             // 環境ブロック
		NULL,             // カレントディレクトリ
		&si,              // STARTUPINFO
		&pi               // PROCESS_INFORMATION
	);

	if (!success)
	{
		Debug::ErrorLog("Failed to execute process:", command);
		return -1; // エラーコードを返す
	}

	//プロセスが終了するまで待機
	WaitForSingleObject(pi.hProcess, INFINITE);

	//終了コードの取得
	DWORD exitCode;
	GetExitCodeProcess(pi.hProcess, &exitCode);

	//ハンドルを閉じる
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return (int)exitCode;
}

string ScriptHotReloadManager::GetVsWherePath()
{
	char pf_x86[MAX_PATH];
	DWORD len = GetEnvironmentVariableA("ProgramFiles(x86)", pf_x86, MAX_PATH);

	string pathBase;

	if (len == 0 || len >= MAX_PATH)
	{
		// 環境変数が取得できなかった場合（32bit OS や特殊な環境など）
		// デフォルトのパスにフォールバックする
		pathBase = "C:\\Program Files (x86)";
	}
	else
	{
		pathBase = pf_x86;
	}

	return "\"" + pathBase + "\\Microsoft Visual Studio\\Installer\\vswhere.exe\"";
}

string ScriptHotReloadManager::FindMsBuildPath()
{
	// 1. vswhere.exe を実行するコマンドを動的に取得
	const std::string vswhereCmd = GetVsWherePath() +
		" -latest -requires Microsoft.Component.MSBuild -find MSBuild\\**\\Bin\\MSBuild.exe -nologo";

	std::array<char, 1024> buffer;
	string result;

	//2.コマンドを実行し、標準出力をパイプで開く
	UniquePipe pipe(_popen(vswhereCmd.c_str(), "r"));
	if (!pipe)
	{
		throw std::runtime_error("Failed to run vswhere.exe. Is Visual Studio Installer installed?");
	}

	//3.パイプから結果を一行
	if (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
		result = buffer.data();
	}
	else
	{
		throw std::runtime_error("vswhere.exe did not return a path. Is Visual Studio (with MSBuild) installed?");
	}

	// 4. 取得したパス末尾の改行コードを削除
	result.erase(result.find_last_not_of(" \n\r\t") + 1);

	if (result.empty()) {
		throw std::runtime_error("MSBuild.exe not found via vswhere.exe.");
	}

	// 5. パスにスペースが含まれるため引用符で囲む
	return "\"" + result + "\"";
}

bool ScriptHotReloadManager::ExecuteMsbuildAndReload()
{

	string msBuildPath;
	try {
		// 1. msbuild.exe へのフルパスを動的に取得
		msBuildPath = FindMsBuildPath();
	}
	catch (const std::exception& e) {
		
		// MSBuild が見つからなかった
		Debug::ErrorLog("Failed to find MSBuild.exe: ", e.what());
		return false;
	}

	// もしvswhere.exeがなかったら下記のMSBuild.exeまでのフルパスを指定してビルドしてください
	//msBuildPath = "\"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe\"";

	string targetPath = "InGameProject.sln";

	// 2. コマンド文字列をフルパスで組み立てる
	string msBuildCommand = msBuildPath + " " + targetPath + " /p:Configuration=Debug /p:Platform=x64";
	//コマンドの出力をログファイルにリダイレクト（ビルドエラーの詳細を確認するため）
	//string msBuildCommand = "cmd /c " + msBuildPath + " " + targetPath + " /p:Configuration=Debug /p:Platform=x64 > build_log.txt 2>&1";
	int buildResult = ExecuteAndWaitForProcess(msBuildCommand);

	if (buildResult != 0)
	{
		// ビルド失敗（新しく追加したスクリプトにコンパイルエラーがないか確認）
		Debug::ErrorLog("MSBuild failed. InGameProject.dll was NOT regenerated.");
		// DLLが再生成されていないため、ロード処理をスキップする
		return false;
	}

	// ビルド成功した場合のみ、DLLのロードと置き換え（ホットリロード）を実行
	return ReloadInGameProject();
}
