#include "FileOperationManager.h"
#include "SceneManager.h"
#include "DebugManager.h"

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
