#include "FileOperationManager.h"
#include "BaseScene.h"
#include "DebugManager.h"
#include "SceneManager.h"
#include "SceneSerializer.h"
#include "ScriptEditManager.h"

vector<string> FileOperationManager::mDroppedFiles;

void FileOperationManager::Initialize()
{
    // 初期化処理が必要な場合はここに記述
    // COMの初期化
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
}

void FileOperationManager::ShowInExplorer(const std::wstring& filePath)
{
    try
    {
        std::filesystem::path absolutePath =
            std::filesystem::absolute(filePath);

        std::wstring winPath = absolutePath.make_preferred().wstring();

        PIDLIST_ABSOLUTE pidl;
        HRESULT          hr =
            SHParseDisplayName(winPath.c_str(), nullptr, &pidl, 0, nullptr);

        if (SUCCEEDED(hr))
        {
            hr = SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);

            if (FAILED(hr))
            {
                std::cerr
                    << "エクスプローラーでファイルを表示できませんでした。"
                    << std::endl;
            }
            // メモリの解放
            CoTaskMemFree(pidl);
        }
        else
        {
            std::cerr << "ファイルのパスを解析できませんでした。パスが実在する"
                         "か確認してください: "
                      << std::string(winPath.begin(), winPath.end())
                      << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "パス変換中に例外が発生しました:" << e.what() << std::endl;
        return;
    }
}

void FileOperationManager::OpenFile(const filesystem::path& path)
{
    if (!filesystem::exists(path) || filesystem::is_directory(path))
        return;

    string ext = path.extension().string();

    if (ext == ".json")
    {
        // シーンファイルのロード処理
        SceneManager::LoadSceneGUI(path.string());
        Debug::Log("Loaded scene: %s\n", path.string().c_str());
    }
    else if (ext == ".h" || ext == ".cpp" || ext == ".txt")
    {
        // スクリプトやテキストファイルは OS
        // のデフォルトエディタ（VS、VSCode、メモ帳など）で開く ShellExecuteW
        // を使ってスマートに起動します
        ShellExecuteW(NULL, L"open", path.wstring().c_str(), NULL, NULL,
                      SW_SHOWNORMAL);
        Debug::Log("Opened file in external editor: %s\n",
                   path.string().c_str());
    }
    else
    {
        // その他のアセット（画像やサウンド等）も基本はOSのデフォルトアプリに丸投げ
        ShellExecuteW(NULL, L"open", path.wstring().c_str(), NULL, NULL,
                      SW_SHOWNORMAL);
    }
}

void FileOperationManager::Release()
{
    // クリーンアップ処理が必要な場合はここに記述
    // COMのクリーンアップ
    CoUninitialize();
}

void FileOperationManager::ExecuteRename(const std::filesystem::path& oldPath,
                                         const std::string&           newName)
{
    if (!std::filesystem::exists(oldPath) || newName.empty())
        return;
    if (oldPath.filename().stem().string() == newName)
        return; // 名前が変わっていなければスルー

    // 対象がC++スクリプト（.h または .cpp）か判定
    std::string ext = oldPath.extension().string();
    if (ext == ".h" || ext == ".cpp")
    {
        RenameScriptPair(oldPath, newName);
    }
    else
    {
        if (ext == ".json")
        {
            RenameRunSceneName(oldPath, newName);
        }
        RenameNormalFileOrFolder(oldPath, newName);
    }
}

void FileOperationManager::OpenSceneDialog()
{
    // ファイルオープンダイアログのインスタンスを作成
    IFileOpenDialog* pFileOpen = nullptr;
    HRESULT          hr =
        CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER,
                         IID_PPV_ARGS(&pFileOpen));

    if (SUCCEEDED(hr))
    {
        COMDLG_FILTERSPEC filterSpec[] = {{L"Scene Files (*.json)", L"*.json"}};
        pFileOpen->SetFileTypes(ARRAYSIZE(filterSpec), filterSpec);
        pFileOpen->SetFileTypeIndex(1); // デフォルトで最初のフィルタを選択

        // オプション)デフォルトのタイトルを設定
        pFileOpen->SetTitle(L"Open Scene");

        IShellItem* pDefaultFolder = nullptr;

        try
        {
            // "Assets"
            // フォルダの絶対パスを取得し、Windows標準の区切り文字（\）にする
            std::filesystem::path assetsPath =
                std::filesystem::absolute("Assets");
            std::wstring winAssetsPath = assetsPath.make_preferred().wstring();

            // 絶対パスを使って IShellItem を作成する
            HRESULT hrFolder = SHCreateItemFromParsingName(
                winAssetsPath.c_str(), // フルパス文字列
                NULL, IID_PPV_ARGS(&pDefaultFolder));

            if (SUCCEEDED(hrFolder))
            {
                // ダイアログが開いたときの初期フォルダとしてセット
                pFileOpen->SetFolder(pDefaultFolder);
                pDefaultFolder->Release();
            }
        }
        catch (const std::exception& e)
        {
            Debug::Log("Failed to parse default folder path: %s\n", e.what());
        }

        // ダイアログを表示
        hr = pFileOpen->Show(NULL);

        if (SUCCEEDED(hr))
        {
            // ユーザーが選択したファイルのアイテムを取得
            IShellItem* pItem = nullptr;
            hr                = pFileOpen->GetResult(&pItem);

            if (SUCCEEDED(hr))
            {
                // アイテムからファイルの絶対パス（文字列）を取得
                PWSTR pszFilePath = nullptr;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (SUCCEEDED(hr))
                {
                    // ワイド文字列（std::wstring）を通常の std::string に変換
                    std::wstring wPath(pszFilePath);
                    std::string  scenePath(wPath.begin(), wPath.end());

                    SceneManager::LoadSceneGUI(scenePath);
                    Debug::Log("Successfully opened scene via dialog: %s\n",
                               scenePath.c_str());

                    // メモリの解放
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        else
        {
            // キャンセルを押したか、閉じた場合
            Debug::Log("Open Scene dialog was canceled.\n");
        }
        pFileOpen->Release();
    }
    else
    {
        Debug::Log("Failed to create FileOpenDialog instance.\n");
    }
}

void FileOperationManager::AddDroppedFile(const std::string& filePath) 
{
    mDroppedFiles.push_back(filePath);
}

void FileOperationManager::RemoveDroppedFile(const std::string& filePath)
{
    auto it = std::find(mDroppedFiles.begin(), mDroppedFiles.end(), filePath);
    if (it != mDroppedFiles.end())
    {
        mDroppedFiles.erase(it);
    }
}

void FileOperationManager::RenameScriptPair(
    const std::filesystem::path& oldPath, const std::string& newName)
{
    try
    {
        std::filesystem::path parentDir    = oldPath.parent_path();
        std::string           oldClassName = oldPath.stem().string();

        std::filesystem::path oldHPath   = parentDir / (oldClassName + ".h");
        std::filesystem::path oldCppPath = parentDir / (oldClassName + ".cpp");
        std::filesystem::path newHPath   = parentDir / (newName + ".h");
        std::filesystem::path newCppPath = parentDir / (newName + ".cpp");

        // エラーチェック: 変更先が既に存在していないか
        if (std::filesystem::exists(newHPath) ||
            std::filesystem::exists(newCppPath))
        {
            Debug::Log(
                "Rename Failed: A script with that name already exists.\n");
            return;
        }

        // 1. .h ファイルのリネームと内部クラス名の置換
        if (std::filesystem::exists(oldHPath))
        {
            std::filesystem::rename(oldHPath, newHPath);
            ScriptEditManager::GetInstance().ReplaceInFile(
                newHPath, oldClassName, newName);
        }

        // 2. .cpp ファイルのリネームと内部クラス名の置換
        if (std::filesystem::exists(oldCppPath))
        {
            std::filesystem::rename(oldCppPath, newCppPath);
            ScriptEditManager::GetInstance().ReplaceInFile(
                newCppPath, oldClassName, newName);
        }

        Debug::Log("Renamed Script Pair: %s -> %s\n", oldClassName.c_str(),
                   newName.c_str());
    }
    catch (const std::exception& e)
    {
        Debug::Log("Script Rename Error: %s\n", e.what());
    }
}

void FileOperationManager::RenameRunSceneName(
    const std::filesystem::path& oldPath, const std::string& newName)
{
    // SceneSerializer::
    if (oldPath.filename().stem().string() !=
        SceneManager::GetCurrentRunScene()->GetName())
    {
        return;
    }
    SceneSerializer::RenameRunScene(oldPath, newName);
}

void FileOperationManager::RenameNormalFileOrFolder(
    const std::filesystem::path& oldPath, const std::string& newName)
{
    try
    {
        // ファイルなら拡張子を維持、フォルダならそのまま結合
        std::filesystem::path newPath = oldPath.parent_path() / newName;
        if (!std::filesystem::is_directory(oldPath))
        {
            newPath += oldPath.extension().string();
        }

        if (std::filesystem::exists(newPath))
        {
            Debug::Log("Rename Failed: Target path already exists.\n");
            return;
        }

        std::filesystem::rename(oldPath, newPath);
        Debug::Log("Renamed: %s -> %s\n", oldPath.filename().string().c_str(),
                   newPath.filename().string().c_str());
    }
    catch (const std::exception& e)
    {
        Debug::Log("Rename Error: %s\n", e.what());
    }
}