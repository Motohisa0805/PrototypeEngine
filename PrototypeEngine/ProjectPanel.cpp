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
	// �E�C���h�E�ʒu�ƃT�C�Y���Œ�
	ImGui::SetNextWindowPos(ImVec2(width * 0.65f, 30));
	ImGui::SetNextWindowSize(ImVec2(width * 0.15f, (float)height - 25));
	//  �V�����E�B���h�E�̍쐬
	ImGui::Begin("Project", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{
		// ���[�g�f�B���N�g�����w��
        fs::path root = "Assets";
        // Assets �t�H���_�����[�g�m�[�h�ɕ\��
        if (ImGui::TreeNode(root.filename().string().c_str()))
        {
            ShowDirectoryRecursive(root);
            ImGui::TreePop();
        }
	}
	ImGui::End();

    // ==== �t���[���I�����ɍ폜�����s ====
    DeleteFile();
}

void ProjectPanel::ShowDirectoryRecursive(const fs::path& path)
{
	// �w�肳�ꂽ�p�X���f�B���N�g���łȂ��ꍇ�͏I��
    // Assets�t�H���_���̃t�@�C����1������
    for (auto& entry : fs::directory_iterator(path))
    {
        const std::string name = entry.path().filename().string();
		// �t�H���_�\��
        if (entry.is_directory())
        {
            bool open = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);

            // �t�H���_���E�N���b�N�������̃��j���[
            RightClickMenu(entry.path());

            // --- �t�H���_���h���b�O���ɂ��� ---
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                std::string pathStr = entry.path().string();
                ImGui::SetDragDropPayload("FILE_DRAG", pathStr.c_str(), pathStr.size() + 1);
                ImGui::Text("Moving folder: %s", name.c_str());
                ImGui::EndDragDropSource();
            }

            // --- �t�H���_���h���b�v��ɂ��� ---
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
            
			// �t�H���_���̃t�@�C���A�t�H���_���ċA�I�ɒ��ׂ�
            if (open)
            {
                ShowDirectoryRecursive(entry.path());
                ImGui::TreePop();
            }
        }
		// �t�@�C���\��
        else
        {
            // �t�@�C���\��
            if (ImGui::Selectable(entry.path().filename().string().c_str()))
            {
                g_SelectedFile = entry.path().string();
            }

            // �h���b�O�J�n����
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                std::string pathStr = entry.path().string();
                ImGui::SetDragDropPayload("FILE_DRAG", pathStr.c_str(), pathStr.size() + 1);
                ImGui::Text("Moving %s", name.c_str()); // �h���b�O���ɕ\������镶��
                ImGui::EndDragDropSource();
            }

            // �t�@�C���E�N���b�N���j���[
			RightClickMenu(entry.path());
        }

        if (mRenaming && entry.path() == mRenameTarget)
        {
            ImGui::PushID(entry.path().string().c_str());

            // ���̓o�b�t�@�m�ہi�ő�255�������x�j
            char buffer[256];
            strncpy_s(buffer, mRenameBuffer.c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0';

            if (ImGui::InputText("##rename", buffer, sizeof(buffer),
                ImGuiInputTextFlags_EnterReturnsTrue))
            {
                mRenameBuffer = buffer;

                // �V�����t���p�X = �e�f�B���N�g�� + ���͖� + ���̊g���q
                fs::path newPath = entry.path().parent_path() / (mRenameBuffer + entry.path().extension().string());

                if (!fs::exists(newPath)) // �Փ˃`�F�b�N
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
    // �t�@�C���E�N���b�N���j���[
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Open in External Editor"))
        {
            std::string command = "start \"\" \"" + path.string() + "\""; // Windows��p
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
                mDeleteQueue.push_back(path); // �����ł͍폜���Ȃ�
            }
        }
        else
        {
            if (ImGui::MenuItem("Delete File"))
            {
                mDeleteQueue.push_back(path); // �����ł͍폜���Ȃ�
            }
        }

        if (ImGui::MenuItem("Rename"))
        {
            mRenameTarget = path;
            mRenameBuffer = path.stem().string(); // �g���q�����������O�����ҏW�Ώۂ�
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
    mDeleteQueue.clear(); // �L���[����ɂ���
}
