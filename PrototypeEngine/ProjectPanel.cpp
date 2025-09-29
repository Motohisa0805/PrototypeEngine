#include "ProjectPanel.h"

ProjectPanel::ProjectPanel(Renderer* renderer)
	:GUIPanel(renderer)
	, mRenaming(false)
	, mDragDroping(false)
	, mRenameBuffer("")
	, mRenameTarget("")
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
}

void ProjectPanel::Draw(float width, float height, ImTextureRef ref)
{
    if (isResetLayout)
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos));
        ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize));
		isResetLayout = false;
    }
    else
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
    }
    if (ImGui::Begin("Project", nullptr, ImGuiWindowFlags_NoCollapse))
    {
        GUIPanelMenu();
        ImGui::Columns(2); // 2カラムに分割

        // 左カラム = フォルダツリー
        if (ImGui::TreeNode("Assets"))
        {
            // 左クリックで選択中フォルダを更新
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                mCurrentFolder = "Assets";
            }

            AssetsFolderPrivateMenu();
            DrawFolderTree("Assets");
            ImGui::TreePop();
        }
        ImGui::NextColumn();
    
        // 右カラム = 選択中フォルダの中身
        DrawFileView();

        // カラム終了
        ImGui::Columns(1); 
    }
    ImGui::End();

    DrawOverwritePopup();
    ProcessPendingOperations(); // 削除やリネームを処理

	// ドラッグ終了
    if (mDragDroping && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        mDragDroping = false;
    }
}

bool ProjectPanel::AssetsFolderPrivateMenu()
{
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("New Folder"))
        {
            fs::create_directory("Assets/NewFolder");
        }
        if (ImGui::MenuItem("New TextFile"))
        {
            std::ofstream("Assets/NewFile.txt");
        }
        // ここに「Import New Asset」なども追加できる
        ImGui::EndPopup();
    }
    return true;
}

void ProjectPanel::DrawFolderTree(const fs::path& path)
{
    for (auto& entry : fs::directory_iterator(path))
    {
        if (!entry.is_directory()) continue; // フォルダだけ表示

        const fs::path entryPath = entry.path();
        const std::string name = entryPath.filename().string();
        
        if (mRenaming && entryPath == mRenameTarget)
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
                mCurrentFolder = entryPath;
                mSelectedPath = entryPath; // 選択パスを更新
            }

            // 通常のフォルダ用メニュー（削除・リネーム可）
            // 右クリックメニュー（Rename 選択で mRenaming が true になる）
            RightClickMenu(entryPath);
            ShortcutKeyInputFunction(entryPath);
            if (open)
            {
                DrawFolderTree(entryPath);
                ImGui::TreePop();
            }

            DragDropFunction(entryPath);
        }
    }
}

void ProjectPanel::DrawFileView()
{
    if (!fs::exists(mCurrentFolder)) return;

    // === パンくずリスト表示 ===
    fs::path root = "Assets";
    fs::path relative = fs::relative(mCurrentFolder, root);

    // ルート ("Assets") を必ず表示
    if (ImGui::Button("Assets##2"))
    {
        mCurrentFolder = root;
    }

    fs::path temp = root;
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

    for (auto& entry : fs::directory_iterator(mCurrentFolder))
    {
        DrawFileSystemEntry(entry);
    }
}

void ProjectPanel::DrawFileSystemEntry(const fs::directory_entry& entry)
{
    const fs::path entryPath = entry.path();
    const std::string name = entryPath.filename().string();

    // リネーム処理
    if (mRenaming && entryPath == mRenameTarget)
    {
        RenameFunction(entry);
        return; // リネーム中は以降の処理を行わない
    }

    // -- - 通常の表示-- -
    ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_None;
    if (entryPath == mSelectedPath)
    {
        selectableFlags |= ImGuiSelectableFlags_Highlight; // ハイライト表示
    }

    // フォルダ・ファイル選択 (単一クリックでの選択と移動)
    // この処理で、フォルダでもファイルでも mSelectedPath は更新される
    if (ImGui::Selectable(name.c_str(), false, selectableFlags))
    {
        mSelectedPath = entryPath;

        // シングルクリックでフォルダ移動させたくない場合はこのブロックを削除
        if (!entry.is_directory())
        {
            // ファイル選択
            mCurrentFile = entryPath.string();
        }
    }

    // ダブルクリック処理
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        if (entry.is_directory())
        {
            // フォルダの場合のみ移動を実行
            mCurrentFolder = entryPath;

            // ダブルクリックで移動した場合、選択状態も更新する
            mSelectedPath = entryPath;
        }
        else
        {
            // ファイルの場合（外部エディタで開くなどの処理をここに追加）
        }
    }

    // コンテキストメニュー、ショートカット、ドラッグ＆ドロップ
    RightClickMenu(entryPath);
    ShortcutKeyInputFunction(entryPath); // ※引数からpathを渡すように変更
    DragDropFunction(entryPath);
}

bool ProjectPanel::RightClickMenu(const fs::path& path)
{
    // コンテキストメニューは直前に描画したアイテム（TreeNode か Selectable）に紐づく
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Open in External Editor"))
        {
            // Windows の例（将来は SDL_OpenURL 等に置き換える）
            std::string command = "start \"\" \"" + path.string() + "\"";
            system(command.c_str());
        }

        if (fs::is_directory(path))
        {
            if (ImGui::MenuItem("New Folder"))
            {
                // 簡易的に NewFolder を作る (衝突は考慮していない)
                try
                {
                    // 簡易的なユニーク名生成の例
                    std::string uniqueName = "NewFolder";
                    int counter = 1;
                    while (fs::exists(path / uniqueName)) {
                        uniqueName = "NewFolder (" + std::to_string(counter++) + ")";
                    }
                    fs::create_directory(path / uniqueName);

                }
                catch (const std::exception& e) 
                {
                    Debug::Log("Create folder failed: %s\n", e.what()); 
                }
            }
            if (ImGui::MenuItem("Delete Folder"))
            {
                // 即削除はしない。遅延キューに追加する
                mDeleteQueue.push_back(path);
            }
        }
        else
        {
            if (ImGui::MenuItem("Delete File"))
            {
                mDeleteQueue.push_back(path);
            }
        }

        // Rename（フォルダ・ファイルどちらでも可）
        if (ImGui::MenuItem("Rename"))
        {
            mRenameTarget = path;
            // ファイルなら拡張子を除いた stem を編集バッファに、フォルダは full name
            if (fs::is_directory(path))
            {
                mRenameBuffer = path.filename().string();
            }
            else
            {
                mRenameBuffer = path.stem().string();
            }
            mRenaming = true;
        }

        ImGui::EndPopup();
    }

    return false;
}

void ProjectPanel::ShortcutKeyInputFunction(const fs::path& path)
{
    //削除キー
    if (!mSelectedPath.empty() && ImGui::IsKeyPressed(ImGuiKey_Delete))
    {
        mDeleteQueue.push_back(mSelectedPath); //mSelectedPath を使用
    }
    //名前変更キー
    if (!mSelectedPath.empty() && ImGui::IsKeyPressed(ImGuiKey_F2))
    {
        mRenameTarget = mSelectedPath; //mSelectedPath をターゲットに

        // ファイルなら拡張子を除いた stem を編集バッファに、フォルダは full name
        if (fs::is_directory(mSelectedPath))
        {
            mRenameBuffer = mSelectedPath.filename().string();
        }
        else
        {
            mRenameBuffer = mSelectedPath.stem().string();
        }
        mRenaming = true;
    }
}

void ProjectPanel::DragDropFunction(const fs::path& path)
{
    if (!mDragDroping)
    {
        // ドラッグ開始処理（Selectable の近くに置く）
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            mDragDroping = true;
            std::string pathStr = path.string();
            ImGui::SetDragDropPayload("FILE_DRAG", pathStr.c_str(), pathStr.size() + 1);
            ImGui::Text("Moving %s", path.filename().string().c_str());
            ImGui::EndDragDropSource();
        }
    }
    else
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_DRAG"))
            {
                const char* srcPathC = (const char*)payload->Data;
                fs::path src(srcPathC);
                fs::path dst;

                if (fs::is_directory(path))
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
                    if (fs::exists(dst))
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
                            mDragDroping = false;
                            fs::rename(src, dst);
                            Debug::Log("Moved: %s -> %s\n", src.string().c_str(), dst.string().c_str());
                        }
                        catch (const std::exception& e)
                        {
                            Debug::Log("Move failed: %s\n", e.what());
                        }
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    }
}

void ProjectPanel::RenameFunction(const fs::directory_entry entry)
{
    ImGui::PushID(entry.path().string().c_str());

    char buffer[256];
#if defined(_MSC_VER)
    strncpy_s(buffer, mRenameBuffer.c_str(), sizeof(buffer));
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
            req.newStem = std::string(buffer);
        }
        else
        {
            // ファイルは拡張子を維持
            req.newStem = std::string(buffer);
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
                fs::remove(mPendingDst);            // 既存を消す
                fs::rename(mPendingSrc, mPendingDst); // 移動
                Debug::Log("Overwritten: %s -> %s\n", mPendingSrc.string().c_str(), mPendingDst.string().c_str());
            }
            catch (const std::exception& e)
            {
                Debug::Log("Overwrite failed: %s\n", e.what());
            }
            mDragDroping = false;
            mShowOverwritePopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("No", ImVec2(120, 0)))
        {
            mDragDroping = false;
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
            if (!fs::exists(req.oldPath)) continue; // 既に消えていたらスキップ
            fs::path newPath = req.oldPath.parent_path() / (req.newStem + req.oldPath.extension().string());
            if (fs::exists(newPath))
            {
                Debug::Log("Rename failed: %s already exists\n", newPath.string().c_str());
            }
            else
            {
                fs::rename(req.oldPath, newPath);
                Debug::Log("Renamed: %s -> %s\n", req.oldPath.string().c_str(), newPath.string().c_str());
            }
        }
        catch (const std::exception& e)
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
            if (!fs::exists(p)) continue;
            if (fs::is_directory(p))
            {
                fs::remove_all(p);
                Debug::Log("Deleted folder: %s\n", p.string().c_str());
            }
            else
            {
                fs::remove(p);
                Debug::Log("Deleted file: %s\n", p.string().c_str());
            }
        }
        catch (const std::exception& e)
        {
            Debug::Log("Delete failed: %s\n", e.what());
        }
    }
    mDeleteQueue.clear();
}
