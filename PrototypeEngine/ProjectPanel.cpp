#include "ProjectPanel.h"

ProjectPanel::ProjectPanel(Renderer* renderer)
	:GUIPanel(renderer)
	, mRenaming(false)
	, mRenameBuffer("")
	, mRenameTarget("")
	, mDeleteQueue()
{
}

void ProjectPanel::Draw(float width, float height)
{
	// ウインドウ位置とサイズを固定
	ImGui::SetNextWindowPos(ImVec2(width * 0.65f, 30));
	ImGui::SetNextWindowSize(ImVec2(width * 0.15f, (float)height - 25));
	//  新しいウィンドウの作成
	ImGui::Begin("Project", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{
		// ルートディレクトリを指定
        fs::path root = "Assets";
        // Assets フォルダをルートノードに表示
        if (ImGui::TreeNode(root.filename().string().c_str()))
        {
            ShowDirectoryRecursive(root);
            ImGui::TreePop();
        }
	}
	ImGui::End();

    // ==== フレーム終了時に削除を実行 ====
    DeleteFile();
}

void ProjectPanel::ShowDirectoryRecursive(const fs::path& path)
{
	// 指定されたパスがディレクトリでない場合は終了
    // Assetsフォルダ内のファイルを1つずつ処理
    for (auto& entry : fs::directory_iterator(path))
    {
        const std::string name = entry.path().filename().string();
		// フォルダ表示
        if (entry.is_directory())
        {
            bool open = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);

            // フォルダを右クリックした時のメニュー
            RightClickMenu(entry.path());

            // --- フォルダをドラッグ元にする ---
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                std::string pathStr = entry.path().string();
                ImGui::SetDragDropPayload("FILE_DRAG", pathStr.c_str(), pathStr.size() + 1);
                ImGui::Text("Moving folder: %s", name.c_str());
                ImGui::EndDragDropSource();
            }

            // --- フォルダをドロップ先にする ---
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_DRAG"))
                {
                    const char* srcPath = (const char*)payload->Data;
                    fs::path src(srcPath);
                    fs::path dst = entry.path() / src.filename();
                    try
                    {
                        fs::rename(src, dst);
                        printf("Moved folder: %s -> %s\n", src.string().c_str(), dst.string().c_str());
                    }
                    catch (const std::exception& e)
                    {
                        printf("Move failed: %s\n", e.what());
                    }
                }
                ImGui::EndDragDropTarget();
            }
            
			// フォルダ内のファイル、フォルダを再帰的に調べる
            if (open)
            {
                ShowDirectoryRecursive(entry.path());
                ImGui::TreePop();
            }
        }
		// ファイル表示
        else
        {
            // ファイル表示
            if (ImGui::Selectable(entry.path().filename().string().c_str()))
            {
                g_SelectedFile = entry.path().string();
            }

            // ドラッグ開始処理
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                std::string pathStr = entry.path().string();
                ImGui::SetDragDropPayload("FILE_DRAG", pathStr.c_str(), pathStr.size() + 1);
                ImGui::Text("Moving %s", name.c_str()); // ドラッグ中に表示される文字
                ImGui::EndDragDropSource();
            }

            // ファイル右クリックメニュー
			RightClickMenu(entry.path());
        }

        if (mRenaming && entry.path() == mRenameTarget)
        {
            ImGui::PushID(entry.path().string().c_str());

            // 入力バッファ確保（最大255文字程度）
            char buffer[256];
            strncpy_s(buffer, mRenameBuffer.c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0';

            if (ImGui::InputText("##rename", buffer, sizeof(buffer),
                ImGuiInputTextFlags_EnterReturnsTrue))
            {
                mRenameBuffer = buffer;

                // 新しいフルパス = 親ディレクトリ + 入力名 + 元の拡張子
                fs::path newPath = entry.path().parent_path() / (mRenameBuffer + entry.path().extension().string());

                if (!fs::exists(newPath)) // 衝突チェック
                {
                    try
                    {
                        fs::rename(entry.path(), newPath);
                        printf("Renamed: %s -> %s\n", entry.path().string().c_str(), newPath.string().c_str());
                    }
                    catch (const std::exception& e)
                    {
                        printf("Rename failed: %s\n", e.what());
                    }
                }
                else
                {
                    printf("Rename failed: %s already exists\n", newPath.string().c_str());
                }
                mRenaming = false;
            }

            ImGui::PopID();
        }
        /*
        else
        {
            ImGui::TextUnformatted(entry.path().filename().string().c_str());
        }
        */
    }
}

bool ProjectPanel::RightClickMenu(const fs::path& path)
{
    // ファイル右クリックメニュー
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Open in External Editor"))
        {
            std::string command = "start \"\" \"" + path.string() + "\""; // Windows専用
            system(command.c_str());
        }

        if (fs::is_directory(path))
        {
            if (ImGui::MenuItem("New Folder"))
            {
                fs::create_directory(path / "NewFolder");
            }
            if (ImGui::MenuItem("Delete Folder"))
            {
                mDeleteQueue.push_back(path); // ここでは削除しない
            }
        }
        else
        {
            if (ImGui::MenuItem("Delete File"))
            {
                mDeleteQueue.push_back(path); // ここでは削除しない
            }
        }

        if (ImGui::MenuItem("Rename"))
        {
            mRenameTarget = path;
            mRenameBuffer = path.stem().string(); // 拡張子を除いた名前だけ編集対象に
            mRenaming = true;
        }

        ImGui::EndPopup();
    }
	return false;
}

void ProjectPanel::DeleteFile()
{
    for (auto& p : mDeleteQueue)
    {
        try
        {
            if (fs::is_directory(p))
            {
                fs::remove_all(p);
                printf("Deleted folder: %s\n", p.string().c_str());
            }
            else
            {
                fs::remove(p);
                printf("Deleted file: %s\n", p.string().c_str());
            }
        }
        catch (const std::exception& e)
        {
            printf("Delete failed: %s\n", e.what());
        }
    }
    mDeleteQueue.clear(); // キューを空にする
}
