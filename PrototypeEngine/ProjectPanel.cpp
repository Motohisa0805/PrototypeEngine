#include "ProjectPanel.h"
#include "EditorSettingsManager.h"
#include "EditorUtils.h"
#include "SceneManager.h"
#include "SceneSerializer.h"
#include "DebugManager.h"

ProjectPanel::ProjectPanel(Renderer* renderer)
	:GUIPanel(renderer)
	, mRenaming(false)
	, mRenameInputBuffer("")
	, mPathToRename("")
	, mDeleteQueue()
    , mCurrentFolder("Assets")
{
}

void ProjectPanel::Initialize(float width, float height, ImTextureRef ref)
{
    mWidthPos = width * 0.65f;
    mHeightPos = 55.0f;
    mWidthSize = width * 0.15f;
    mHeightSize = height - 55.0f;
    GUIPanel::Initialize(width, height, ref);
}

void ProjectPanel::Draw(float width, float height, ImTextureRef ref)
{
    float panel1_SizeWidth = mWidthSize / 2.0f;
    // ウインドウ位置とサイズを固定
    if (isResetLayout)
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos));
        ImGui::SetNextWindowSize(ImVec2(panel1_SizeWidth, mHeightSize));
    }
    else
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(panel1_SizeWidth, mHeightSize), ImGuiCond_Once);
    }
    if (ImGui::Begin("FolderTree", nullptr, ImGuiWindowFlags_NoCollapse))
    {
        // 左カラム = フォルダツリー
        if (ImGui::TreeNode("Assets"))
        {
            // 左クリックで選択中フォルダを更新(Assetsフォルダ用)
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                mCurrentFolder = "Assets";
            }
            RightClickMenu("Assets");
            //フォルダツリー表示
            DrawFolderTree("Assets");
            ImGui::TreePop();
        }
    }
    ImGui::End();
    // ウインドウ位置とサイズを固定
    if (isResetLayout)
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos + panel1_SizeWidth, mHeightPos));
        ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize));
        isResetLayout = false;
    }
    else
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos + panel1_SizeWidth, mHeightPos), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(panel1_SizeWidth, mHeightSize), ImGuiCond_Once);
    }
    if (ImGui::Begin(mCurrentFolder.string().c_str(), nullptr, ImGuiWindowFlags_NoCollapse))
    {
        //選択中フォルダの中身表示
        DrawPickUpFolderView();
    }
    ImGui::End();

    DrawOverwritePopup();
    // 削除やリネームを処理
    ProcessPendingOperations(); 
}

bool ProjectPanel::AssetsFolderPrivateMenu()
{
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("New Folder"))
        {
            filesystem::create_directory("Assets/NewFolder");
        }
        if (ImGui::MenuItem("New TextFile"))
        {
            ofstream("Assets/NewFile.txt");
        }
        // ここに「Import New Asset」なども追加できる
        ImGui::EndPopup();
    }
    return true;
}

void ProjectPanel::DrawFolderTree(const filesystem::path& path)
{
    for (auto& entry : filesystem::directory_iterator(path))
    {
        if (!entry.is_directory()) continue; // フォルダだけ表示

        const string name = entry.path().filename().string();
        
        //名前変更処理
        if (mRenaming && entry.path() == mPathToRename)
        {
            RenameFunction(entry);
        }
        else
        {
            // ツリーノードの表示
            // ImGuiTreeNodeFlags_Selected: mSelectedPathと一致する場合にハイライト表示させる
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
            if (path == mSelectedPath)
            {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            bool open = ImGui::TreeNodeEx(name.c_str(), flags); // 修正: flagsを使用

            // 左クリックで選択中フォルダを更新
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                mCurrentFolder = entry.path();
                mSelectedPath = entry.path(); // 選択パスを更新
            }

            // 通常のフォルダ用メニュー（削除・リネーム可）
            // 右クリックメニュー（Rename 選択で mRenaming が true になる）
            RightClickMenu(entry.path());
            ShortcutKeyInputFunction(entry.path());
            if (open)
            {
                DrawFolderTree(entry.path());
                ImGui::TreePop();
            }

            DragDropFunction(entry.path());
        }
    }
}

void ProjectPanel::DrawPickUpFolderView()
{
    if (!filesystem::exists(mCurrentFolder)) return;

    // === パンくずリスト表示 ===
    filesystem::path root = "Assets";
    filesystem::path relative = filesystem::relative(mCurrentFolder, root);

    // ルート ("Assets") を必ず表示
    if (ImGui::Button("Assets##2"))
    {
        mCurrentFolder = root;
    }

    filesystem::path temp = root;
    for (auto& part : relative)
    {
        ImGui::SameLine();
        ImGui::Text(">");
        ImGui::SameLine();

        temp /= part;
        if (ImGui::Button(part.string().c_str()))
        {
            mCurrentFolder = temp;
        }
    }

    ImGui::Separator(); // パンくずとファイルリストを区切る

    for (auto& entry : filesystem::directory_iterator(mCurrentFolder))
    {
        DrawFileSystemEntry(entry);
    }

    if (ImGui::BeginPopupContextWindow())
    {
        filesystem::path targetPath = mCurrentFolder;
        //フォルダ
        if (ImGui::MenuItem("New Folder"))
        {
            // 簡易的に NewFolder を作る (衝突は考慮していない)
            try
            {
                // 簡易的なユニーク名生成の例
                std::string uniqueName = "NewFolder";
                int counter = 1;
                while (filesystem::exists(targetPath / uniqueName)) {
                    uniqueName = "NewFolder (" + std::to_string(counter++) + ")";
                }
                filesystem::create_directory(targetPath / uniqueName);

            }
            catch (const exception& e)
            {
                Debug::Log("Create folder failed: %s\n", e.what());
            }
        }
        //シーン作成
        if (ImGui::MenuItem("New Scene"))
        {
            string uniqueName = "NewScene.json"; // 拡張子付きで初期化
            filesystem::path targetFolder = targetPath; // 現在右クリックしているパス（フォルダ）

            // 既に存在するファイル名かチェックし、ユニークな名前に変更する
            int counter = 1;
            while (filesystem::exists(targetFolder / uniqueName)) {
                // NewScene(1).json, NewScene(2).json のように生成
                uniqueName = "NewScene (" + std::to_string(counter++) + ").json";
            }

            filesystem::path newScenePath = targetFolder / uniqueName;

            // 3. SceneSerializerを使って空のシーンデータをファイルに書き出す
            // SceneSerializer::SaveEmptyScene()内でファイル書き込み処理を行う
            if (SceneSerializer::SaveEmptyScene(newScenePath))
            {
                // 成功ログ
                Debug::Log("Created new scene: %s\n", newScenePath.string().c_str());
            }
            else
            {
                // 失敗ログ
                Debug::Log("Failed to create scene file: %s\n", newScenePath.string().c_str());
            }
        }
        //Script作成
        if (ImGui::MenuItem("New Script (C++)"))
        {

            string uniqueName = "NewScript";
            //現在右クリックしているフォルダ
            filesystem::path targetFolder = targetPath;

            //既に存在するファイル名かチェックし、ユニークな名前に変更する
            int counter = 1;
            while (filesystem::exists(targetFolder / (uniqueName + ".h")))
            {
                uniqueName = "NewScript" + std::to_string(counter++);
            }

            //ヘルパー関数を呼び出してファイルを生成(.hと.cpp)
            if (EditorUtils::GetInstance().CreateScriptFile(targetFolder, uniqueName))
            {
                //成功した場合、Unityのように即座にリネーム状態に移行する
                mSelectedPath = targetFolder / (uniqueName + ".h");
                mPathToRename = targetFolder / (uniqueName + ".h");
                mRenameInputBuffer = uniqueName;//拡張子なしの名前をバッファに
                mRenaming = true;
                Debug::Log("Created new script: %s\n", uniqueName.c_str());
            }
        }
        ImGui::EndPopup();
    }
}

void ProjectPanel::DrawFileSystemEntry(const filesystem::directory_entry& entry)
{
    const string name = entry.path().filename().string();

    // リネーム処理
    if (mRenaming && entry.path() == mPathToRename)
    {
        RenameFunction(entry);
        return; // リネーム中は以降の処理を行わない
    }

    // -- - 通常の表示-- -
    ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_None;
    if (entry.path() == mSelectedPath)
    {
        selectableFlags |= ImGuiSelectableFlags_Highlight; // ハイライト表示
    }

    // フォルダ・ファイル選択 (単一クリックでの選択と移動)
    // この処理で、フォルダでもファイルでも mSelectedPath は更新される
    if (ImGui::Selectable(name.c_str(), false, selectableFlags))
    {
        mSelectedPath = entry.path();

        // シングルクリックでフォルダ移動させたくない場合はこのブロックを削除
        if (!entry.is_directory())
        {
            // ファイル選択
            mCurrentFile = entry.path().string();
        }
    }

    // ダブルクリック処理
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        if (entry.is_directory())
        {
            // フォルダの場合のみ移動を実行
            mCurrentFolder = entry.path();

            // ダブルクリックで移動した場合、選択状態も更新する
            mSelectedPath = entry.path();
        }
        else
        {
            // ファイルの場合
            if (entry.path().extension().string() == ".json")
            {
                // シーンファイルのロード処理を呼び出す
                // 実行中のシーンと切り替えるため、SceneManagerに処理を依頼します
                SceneManager::LoadSceneFromFile(entry.path().string());
                EditorSettingsManager::GetInstance().SetLastOpenedScene(entry.path().string());
            }
            else
            {
                // その他のファイルの場合（外部エディタで開くなど）
            }
        }
    }
    //右クリック処理
    RightClickMenu(entry.path());
    // コンテキストメニュー、ショートカット、ドラッグ＆ドロップ
    ShortcutKeyInputFunction(entry.path()); 
    DragDropFunction(entry.path());
}

bool ProjectPanel::RightClickMenu(const filesystem::path& path)
{
    // コンテキストメニューは直前に描画したアイテム（TreeNode か Selectable）に紐づく
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Open in External Editor"))
        {
            // Windows の例（将来は SDL_OpenURL 等に置き換える）
            string command = "start \"\" \"" + path.string() + "\"";
            system(command.c_str());
        }

        // Rename（フォルダ・ファイルどちらでも可）
        if (ImGui::MenuItem("Rename"))
        {
            mPathToRename = path;
            // ファイルなら拡張子を除いた stem を編集バッファに、フォルダは full name
            if (filesystem::is_directory(path))
            {
                mRenameInputBuffer = path.filename().string();
            }
            else
            {
                mRenameInputBuffer = path.stem().string();
            }
            mRenaming = true;
        }
        //区切り
        ImGui::Separator();


        if (filesystem::is_directory(path))
        {
            //フォルダ
            if (ImGui::MenuItem("New Folder"))
            {
                // 簡易的に NewFolder を作る (衝突は考慮していない)
                try
                {
                    // 簡易的なユニーク名生成の例
                    std::string uniqueName = "NewFolder";
                    int counter = 1;
                    while (filesystem::exists(path / uniqueName)) {
                        uniqueName = "NewFolder (" + std::to_string(counter++) + ")";
                    }
                    filesystem::create_directory(path / uniqueName);

                }
                catch (const exception& e) 
                {
                    Debug::Log("Create folder failed: %s\n", e.what()); 
                }
            }
            //シーン作成
            if (ImGui::MenuItem("New Scene"))
            {
                string uniqueName = "NewScene.json"; // 拡張子付きで初期化
                filesystem::path targetFolder = path; // 現在右クリックしているパス（フォルダ）

                // 既に存在するファイル名かチェックし、ユニークな名前に変更する
                int counter = 1;
                while (filesystem::exists(targetFolder / uniqueName)) {
                    // NewScene(1).json, NewScene(2).json のように生成
                    uniqueName = "NewScene (" + std::to_string(counter++) + ").json";
                }

                filesystem::path newScenePath = targetFolder / uniqueName;

                // 3. SceneSerializerを使って空のシーンデータをファイルに書き出す
                // SceneSerializer::SaveEmptyScene()内でファイル書き込み処理を行う
                if (SceneSerializer::SaveEmptyScene(newScenePath))
                {
                    // 成功ログ
                    Debug::Log("Created new scene: %s\n", newScenePath.string().c_str());
                }
                else
                {
                    // 失敗ログ
                    Debug::Log("Failed to create scene file: %s\n", newScenePath.string().c_str());
                }
            }
            //Script作成
            if (ImGui::MenuItem("New Script (C++)"))
            {

                string uniqueName = "NewScript";
                //現在右クリックしているフォルダ
                filesystem::path targetFolder = path;

                //既に存在するファイル名かチェックし、ユニークな名前に変更する
                int counter = 1;
                while (filesystem::exists(targetFolder / (uniqueName + ".h"))) 
                {
                    uniqueName = "NewScript" + std::to_string(counter++);
                }

                //ヘルパー関数を呼び出してファイルを生成(.hと.cpp)
                if (EditorUtils::GetInstance().CreateScriptFile(targetFolder, uniqueName))
                {
                    //成功した場合、Unityのように即座にリネーム状態に移行する
                    mSelectedPath = targetFolder / (uniqueName + ".h");
                    mPathToRename = targetFolder / (uniqueName + ".h");
                    mRenameInputBuffer = uniqueName;//拡張子なしの名前をバッファに
                    mRenaming = true;
                    Debug::Log("Created new script: %s\n", uniqueName.c_str());
                }
            }
            if (path.string() != "Assets")
            {
                //フォルダの削除
                if (ImGui::MenuItem("Delete Folder"))
                {
                    // 即削除はしない。遅延キューに追加する
                    mDeleteQueue.push_back(path);
                }
            }
        }
        else
        {
            if (ImGui::MenuItem("Delete File"))
            {
                mDeleteQueue.push_back(path);
            }
        }

        ImGui::EndPopup();
        return true;
    }
    return false;
}

void ProjectPanel::ShortcutKeyInputFunction(const filesystem::path& path)
{
    //削除キー
    if (!mSelectedPath.empty() && ImGui::IsKeyPressed(ImGuiKey_Delete))
    {
        mDeleteQueue.push_back(mSelectedPath); //mSelectedPath を使用
    }
    //名前変更キー
    if (!mSelectedPath.empty() && ImGui::IsKeyPressed(ImGuiKey_F2))
    {
        mPathToRename = mSelectedPath; //mSelectedPath をターゲットに

        // ファイルなら拡張子を除いた stem を編集バッファに、フォルダは full name
        if (filesystem::is_directory(mSelectedPath))
        {
            mRenameInputBuffer = mSelectedPath.filename().string();
        }
        else
        {
            mRenameInputBuffer = mSelectedPath.stem().string();
        }
        mRenaming = true;
    }
}

void ProjectPanel::DragDropFunction(const filesystem::path& path)
{
    const string& filePath = path.string();
    // ドラッグ開始処理（Selectable の近くに置く）
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", filePath.c_str(), filePath.size() + 1);
        // ドラッグ中の表示
        ImGui::Text("%s", path.filename().string().c_str());
        ImGui::EndDragDropSource();
    }
    //ドラッグ終了処理
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
        {
            const char* srcPathC = (const char*)payload->Data;
            filesystem::path src(srcPathC);
            filesystem::path dst;

            if (filesystem::is_directory(path))
            {
                // フォルダにドロップ → 中に入れる
                dst = path / src.filename();
            }
            else
            {
                // ファイルにドロップ → 同じ階層
                dst = path.parent_path() / src.filename();
            }

            if (src != dst && !src.string().starts_with(dst.string()))
            {
                if (filesystem::exists(dst))
                {
                    // すでに存在 → 上書き確認ダイアログへ
                    mPendingSrc = src;
                    mPendingDst = dst;
                    mShowOverwritePopup = true;
                    ImGui::OpenPopup("Overwrite?");
                }
                else
                {
                    try
                    {
                        filesystem::rename(src, dst);
                    }
                    catch (const exception& e)
                    {
                        Debug::Log("Move failed: %s\n", e.what());
                    }
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
}

void ProjectPanel::RenameFunction(const filesystem::directory_entry entry)
{
    ImGui::PushID(entry.path().string().c_str());

    char buffer[256];
    //ここで入力を行っている
#if defined(_MSC_VER)
    strncpy_s(buffer, mRenameInputBuffer.c_str(), sizeof(buffer));
#else
    std::strncpy(buffer, mRenameBuffer.c_str(), sizeof(buffer));
#endif
    buffer[sizeof(buffer) - 1] = '\0';

    if (ImGui::InputText("##rename", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        RenameRequest req;
        req.oldPath = entry.path();

        if (entry.is_directory())
        {
            // フォルダはそのまま
            req.newStem = string(buffer);
        }
        else
        {
            // ファイルは拡張子を維持
            req.newStem = string(buffer);
        }

        mRenameQueue.push_back(req);
        mRenaming = false;
    }

    // Esc キャンセル
    if (ImGui::IsItemDeactivated() && !ImGui::IsItemDeactivatedAfterEdit())
    {
        mRenaming = false;
    }

    ImGui::PopID();
}

void ProjectPanel::DrawOverwritePopup()
{
    if (ImGui::BeginPopupModal("Overwrite?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("File already exists:\n%s\n\nOverwrite?", mPendingDst.string().c_str());

        if (ImGui::Button("Yes", ImVec2(120, 0)))
        {
            try
            {
                filesystem::remove(mPendingDst);            // 既存を消す
                filesystem::rename(mPendingSrc, mPendingDst); // 移動
                Debug::Log("Overwritten: %s -> %s\n", mPendingSrc.string().c_str(), mPendingDst.string().c_str());
            }
            catch (const exception& e)
            {
                Debug::Log("Overwrite failed: %s\n", e.what());
            }
            mShowOverwritePopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("No", ImVec2(120, 0)))
        {
            mShowOverwritePopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ProjectPanel::ProcessPendingOperations()
{
    // まずリネームを行う（リネーム後の名前衝突チェックを行う）
    for (const auto& req : mRenameQueue)
    {
        try
        {
            if (!filesystem::exists(req.oldPath))continue;
            
            //名前前のクラス名と名前変更後のクラス名を特定
            string oldClassName = req.oldPath.stem().string();//例："NewScript"
            string newClassName = req.newStem;                //例："OrignalScript"  

            //C++のスクリプトファイルペアかどうかを判定
            bool isScriptFile = (req.oldPath.extension() == ".h" || req.oldPath.extension() == ".cpp");

            if (isScriptFile && oldClassName != newClassName)
            {
                // ----------------------------------------------------
                // 拡張: ファイルシステム上で、対応する .h と .cpp の両方をリネームする
                // ----------------------------------------------------

                //変更前のパスを計算
                filesystem::path oldHPath = req.oldPath.parent_path() / (oldClassName + ".h");
                filesystem::path oldCppPath = req.oldPath.parent_path() / (oldClassName + ".cpp");

                //変更後のパスを計算
                filesystem::path newHPath = req.oldPath.parent_path() / (newClassName + ".h");
                filesystem::path newCppPath = req.oldPath.parent_path() / (newClassName + ".cpp");

                //1..hファイルのリネーム
                if (filesystem::exists(oldHPath))
                {
                    filesystem::rename(oldHPath, newHPath);
                    //2..hファイルの内容を書き換え
                    EditorUtils::GetInstance().ReplaceInFile(newHPath, oldClassName, newClassName);
                }

                //3..cppファイルのリネーム
                if (filesystem::exists(oldCppPath))
                {
                    filesystem::rename(oldCppPath, newCppPath);
                    //2..cppファイルの内容を書き換え
                    EditorUtils::GetInstance().ReplaceInFile(newCppPath, oldClassName, newClassName);
                }
                // 4. vcxProjのスクリプトエントリーを更新
                if (EditorUtils::GetInstance().RemoveScriptFileToVcxProj(oldHPath, oldClassName))
                {
                    Debug::Log("Successfully removed old vcxproj entry: %s.cpp\n", oldClassName.c_str());
                }
                if (EditorUtils::GetInstance().AddScriptFileToVcxProj(newHPath, newClassName))
                {
                    Debug::Log("Successfully added %s.cpp to %s.\n",newClassName.c_str(), EditorUtils::GetInstance().GetVcxppoj_Path().string().c_str());
                }
                else
                {
                    Debug::Log("Failed to add %s.cpp to vcxproj.Maunal addition required.\n",newClassName.c_str(),newClassName.c_str());
                }
            }
            else
            {
                // スクリプトファイルでない、または名前が変わっていない場合は、元のロジックに従い、
                // req.oldPath のファイルのみをリネームキューの定義通りに処理する
                filesystem::path newPath = req.oldPath.parent_path() / (req.newStem + req.oldPath.extension().string());
                if (!filesystem::exists(newPath))
                {
                    filesystem::rename(req.oldPath, newPath);
                    Debug::Log("Renamed: %s -> %s\\n", req.oldPath.string().c_str(), newPath.string().c_str());
                }
            }
        }
        catch (const exception& e)
        {
            Debug::Log("Rename failed: %s\n", e.what());
        }
    }
    mRenameQueue.clear();

    // 次に削除処理
    for (const auto& p : mDeleteQueue)
    {
        try
        {
            if (!filesystem::exists(p)) continue;
            // 1. スクリプトファイルの削除 (vcpjrojから削除)
            if (p.extension() == ".h" || p.extension() == ".cpp")
            {
                // スクリプト名（拡張子なし）を取得
                std::string scriptClassName = p.stem().string();

                // .h, .cpp のどちらか一つがキューに入っていれば、
                // EditorUtils側で両方のパスを計算し、vcxprojから両方削除する。
                // （ファイルシステムの削除はEditorUtilsではなく、このループで実行）
                // ただし、もし .h と .cpp が両方キューに入っている場合、二重にvcxproj処理が走るが、
                // tinyxml2::DeleteChild は要素が見つからなければ何もしないので安全。
                EditorUtils::GetInstance().RemoveScriptFileToVcxProj(p, scriptClassName);
            }

            // 2. ファイルシステムからの削除
            if (filesystem::is_directory(p))
            {
                // フォルダの場合、配下のすべてのファイルを削除
                filesystem::remove_all(p);
                Debug::Log("Deleted folder: %s\n", p.string().c_str());
            }
            else // ファイルの場合
            {
                filesystem::remove(p);
                Debug::Log("Deleted file: %s\n", p.string().c_str());
            }
        }
        catch (const exception& e)
        {
            Debug::Log("Delete failed: %s\n", e.what());
        }
    }
    mDeleteQueue.clear();
}
