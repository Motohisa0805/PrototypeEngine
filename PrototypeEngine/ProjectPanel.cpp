#include "ProjectPanel.h"

ProjectPanel::ProjectPanel(Renderer* renderer)
	:GUIPanel(renderer)
	, mRenaming(false)
	, mRenameBuffer("")
	, mRenameTarget("")
	, mDeleteQueue()
    , mCurrentFolder("Assets")
{
}

void ProjectPanel::Draw(float width, float height)
{
    ImGui::SetNextWindowPos(ImVec2(width * 0.65f, 30));
    ImGui::SetNextWindowSize(ImVec2(width * 0.15f, (float)height - 25));
    ImGui::Begin("Project", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    ImGui::Columns(2); // 2カラムに分割

    // 左カラム = フォルダツリー
    if (ImGui::TreeNode("Assets"))
    {
        DrawFolderTree("Assets");
        ImGui::TreePop();
    }
    ImGui::NextColumn();

    // 右カラム = 選択中フォルダの中身
    DrawFileView();

    ImGui::Columns(1); // カラム終了

    ImGui::End();

    ProcessPendingOperations(); // 削除やリネームを処理
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
            // === フォルダ名リネームモード ===
            ImGui::PushID(entryPath.string().c_str());

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
                req.oldPath = entryPath;
                req.newStem = std::string(buffer); // フォルダは stem の代わりに新しい名前をそのまま使う
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
        else
        {
            // === 通常のフォルダ表示 ===
            bool open = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);

            // 左クリックで選択中フォルダを更新
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                mCurrentFolder = entryPath;
            }

            // 右クリックメニュー（Rename 選択で mRenaming が true になる）
            RightClickMenu(entryPath);

            if (open)
            {
                DrawFolderTree(entryPath);
                ImGui::TreePop();
            }


			DragDropTarget(entryPath);
            
        }
    }
}

void ProjectPanel::DrawFileView()
{
    if (!fs::exists(mCurrentFolder)) return;

    for (auto& entry : fs::directory_iterator(mCurrentFolder))
    {
        const fs::path entryPath = entry.path();
        const std::string name = entryPath.filename().string();

        // --- リネーム対象なら InputText に切り替え ---
        if (mRenaming && entryPath == mRenameTarget)
        {
            ImGui::PushID(entryPath.string().c_str());

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
                req.oldPath = entryPath;

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
        else
        {
            // --- 通常の表示 ---
            if (ImGui::Selectable(name.c_str()))
            {
                if (entry.is_directory())
                {
                    // フォルダクリックで開く
                    mCurrentFolder = entryPath;
                }
                else
                {
                    // ファイル選択
                    g_SelectedFile = entryPath.string();
                }
            }

            // コンテキストメニュー
			RightClickMenu(entryPath);


            // ドラッグ開始処理（Selectable の近くに置く）
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                std::string pathStr = entryPath.string();
                ImGui::SetDragDropPayload("FILE_DRAG", pathStr.c_str(), pathStr.size() + 1);
                ImGui::Text("Moving %s", name.c_str());
                ImGui::EndDragDropSource();
            }
        }
    }
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
                try { fs::create_directory(path / "NewFolder"); }
                catch (const std::exception& e) { Debug::Log("Create folder failed: %s\n", e.what()); }
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

void ProjectPanel::DragDropTarget(const fs::path& path)
{
    const std::string name = path.filename().string();

    // ドロップ先にする
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_DRAG"))
        {
            const char* srcPathC = (const char*)payload->Data;
            fs::path src(srcPathC);
            fs::path dst = path / src.filename();

            // 自分自身や子孫への移動は禁止
            if (src != path && !src.string().starts_with(path.string()))
            {
                try
                {
                    fs::rename(src, dst);
                    Debug::Log("Moved: %s -> %s\n", src.string().c_str(), dst.string().c_str());
                }
                catch (const std::exception& e)
                {
                    Debug::Log("Move failed: %s\n", e.what());
                }
            }
        }
        ImGui::EndDragDropTarget();
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
