#include "FileOperationManager.h"
#include "SceneManager.h"
#include "DebugManager.h"
#include "ScriptEditManager.h"

void FileOperationManager::Initialize()
{
	// 初期化処理が必要な場合はここに記述
	// COMの初期化
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
}

void FileOperationManager::ShowInExplorer(const std::wstring& filePath)
{
	try {
		std::filesystem::path absolutePath = std::filesystem::absolute(filePath);

		std::wstring winPath = absolutePath.make_preferred().wstring();
		
		PIDLIST_ABSOLUTE pidl;
		HRESULT hr = SHParseDisplayName(winPath.c_str(), nullptr, &pidl, 0, nullptr);

		if (SUCCEEDED(hr)) {
			hr = SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);

			if (FAILED(hr)) {
				std::cerr << "エクスプローラーでファイルを表示できませんでした。" << std::endl;
			}
			// メモリの解放
			CoTaskMemFree(pidl);
		}
		else {
			std::cerr << "ファイルのパスを解析できませんでした。パスが実在するか確認してください: "
				<< std::string(winPath.begin(), winPath.end()) << std::endl;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "パス変換中に例外が発生しました:" << e.what() << std::endl;
		return;
	}
}

void FileOperationManager::OpenFile(const filesystem::path& path)
{
	if (!filesystem::exists(path) || filesystem::is_directory(path))return;

	string ext = path.extension().string();

	if (ext == ".json")
	{
		// シーンファイルのロード処理
		SceneManager::LoadSceneGUI(path.string());
		Debug::Log("Loaded scene: %s\n", path.string().c_str());
	}
	else if (ext == ".h" || ext == ".cpp" || ext == ".txt")
	{
		// スクリプトやテキストファイルは OS のデフォルトエディタ（VS、VSCode、メモ帳など）で開く
		// ShellExecuteW を使ってスマートに起動します
		ShellExecuteW(NULL, L"open", path.wstring().c_str(), NULL, NULL, SW_SHOWNORMAL);
		Debug::Log("Opened file in external editor: %s\n", path.string().c_str());
	}
	else
	{
		// その他のアセット（画像やサウンド等）も基本はOSのデフォルトアプリに丸投げ
		ShellExecuteW(NULL, L"open", path.wstring().c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}

void FileOperationManager::Release()
{
	// クリーンアップ処理が必要な場合はここに記述
	// COMのクリーンアップ
	CoUninitialize();
}

void FileOperationManager::ExecuteRename(const std::filesystem::path& oldPath, const std::string& newName)
{
	if (!std::filesystem::exists(oldPath) || newName.empty()) return;
	if (oldPath.filename().stem().string() == newName) return; // 名前が変わっていなければスルー

	// 対象がC++スクリプト（.h または .cpp）か判定
	std::string ext = oldPath.extension().string();
	if (ext == ".h" || ext == ".cpp")
	{
		RenameScriptPair(oldPath, newName);
	}
	else
	{
		RenameNormalFileOrFolder(oldPath, newName);
	}
}

void FileOperationManager::RenameScriptPair(const std::filesystem::path& oldPath, const std::string& newName)
{
	try
	{
		std::filesystem::path parentDir = oldPath.parent_path();
		std::string oldClassName = oldPath.stem().string();

		std::filesystem::path oldHPath = parentDir / (oldClassName + ".h");
		std::filesystem::path oldCppPath = parentDir / (oldClassName + ".cpp");
		std::filesystem::path newHPath = parentDir / (newName + ".h");
		std::filesystem::path newCppPath = parentDir / (newName + ".cpp");

		// エラーチェック: 変更先が既に存在していないか
		if (std::filesystem::exists(newHPath) || std::filesystem::exists(newCppPath)) {
			Debug::Log("Rename Failed: A script with that name already exists.\n");
			return;
		}

		// 1. .h ファイルのリネームと内部クラス名の置換
		if (std::filesystem::exists(oldHPath)) {
			std::filesystem::rename(oldHPath, newHPath);
			ScriptEditManager::GetInstance().ReplaceInFile(newHPath, oldClassName, newName);
		}

		// 2. .cpp ファイルのリネームと内部クラス名の置換
		if (std::filesystem::exists(oldCppPath)) {
			std::filesystem::rename(oldCppPath, newCppPath);
			ScriptEditManager::GetInstance().ReplaceInFile(newCppPath, oldClassName, newName);
		}

		Debug::Log("Renamed Script Pair: %s -> %s\n", oldClassName.c_str(), newName.c_str());
	}
	catch (const std::exception& e) {
		Debug::Log("Script Rename Error: %s\n", e.what());
	}
}

void FileOperationManager::RenameNormalFileOrFolder(const std::filesystem::path& oldPath, const std::string& newName)
{
	try
	{
		// ファイルなら拡張子を維持、フォルダならそのまま結合
		std::filesystem::path newPath = oldPath.parent_path() / newName;
		if (!std::filesystem::is_directory(oldPath)) {
			newPath += oldPath.extension().string();
		}

		if (std::filesystem::exists(newPath)) {
			Debug::Log("Rename Failed: Target path already exists.\n");
			return;
		}

		std::filesystem::rename(oldPath, newPath);
		Debug::Log("Renamed: %s -> %s\n", oldPath.filename().string().c_str(), newPath.filename().string().c_str());
	}
	catch (const std::exception& e) {
		Debug::Log("Rename Error: %s\n", e.what());
	}
}