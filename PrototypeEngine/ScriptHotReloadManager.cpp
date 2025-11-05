#include "ScriptHotReloadManager.h"
#include "SceneManager.h"
#include "ScriptComponent.h"
#include "EditorUtils.h"
#include "ProjectPanel.h"

ScriptHotReloadManager::ScriptHotReloadManager()
	: mSourceDllPath("bin/InGameProject.dll")
	, mActiveDllPath("bin/InGameProject_Active.dll")
	, mSourcePDBPath("bin/InGameProject.pdb")
	, mActivePDBPath("bin/InGameProject_Active.pdb")
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
	string initialDllPath = "bin/InGameProject_Active.dll";

	//初回起動時、オリジナルDLLをアクティブDLLとしてコピー
	if(!CopyFileA(sourcePath.c_str(), initialDllPath.c_str(), FALSE))
	{
		Debug::ErrorLog("File Copy Dll File");
		return false;
	}

	mActiveDllPath = initialDllPath;

	// 2. PDBの初回コピー
	const string& sourcePDBPath = mSourcePDBPath;
	// 初回ロードに使うコピー先のファイルパス (DLLと一致させる)
	string initialPDBPath = "bin/InGameProject_Active.pdb";

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
	mDllIndex = (mDllIndex % 2) + 1; // 1または2を交互に使用
	string newDllPath = "bin/InGameProject_Active.dll";
	
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
	string newPDBPath = "bin/InGameProject_Active.pdb";

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
			string path = entry.path().string();
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
	// --- 2. 削除されたファイルを検出 ---
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

	// --- 3. 変更を処理 ---

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
		std::string stem = std::filesystem::path(path).stem().string();
		if (processedStems.find(stem) == processedStems.end())
		{
			EditorUtils::GetInstance().RemoveScriptFileToVcxProj(path, stem);
			processedStems.insert(stem);
			needsRebuild = true;
		}
	}

	// B) 追加・変更処理
	for (const auto& path : assetsAddedOrModified)
	{
		std::string stem = std::filesystem::path(path).stem().string();
		if (processedStems.find(stem) == processedStems.end())
		{
			EditorUtils::GetInstance().AddScriptFileToVcxProj(path, stem);
			processedStems.insert(stem);
			needsRebuild = true;
		}
	}
	if (needsRebuild)
	{
		return ExecuteMsbuildAndReload();
	}

	// 既存の .dll の変更監視
	FILETIME currentWriteTime = GetDllLastWriteTime(mSourceDllPath);
	if (CompareFileTime(&currentWriteTime, &mLastLoadTime) > 0)
	{
		return ExecuteMsbuildAndReload();
	}
	/*
	*/

	return false;
}

bool ScriptHotReloadManager::ExecuteMsbuildAndReload()
{
	// 1. msbuild.exe へのフルパスを定義
	//    お使いの環境（Community, Professionalなど）やインストール先Cドライブに合わせてください。
	string msBuildPath = "\"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe\"";

	string targetPath = "InGameProject.sln";

	// 2. コマンド文字列をフルパスで組み立てる
	string msBuildCommand = msBuildPath + " " + targetPath + " /p:Configuration=Debug /p:Platform=x64";

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
	if(!GetFileTime(hFile, NULL, NULL, &lastWriteTime))
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
