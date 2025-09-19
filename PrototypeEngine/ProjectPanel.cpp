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

    ImGui::Columns(2); // 2�J�����ɕ���

    // ���J���� = �t�H���_�c���[
    if (ImGui::TreeNode("Assets"))
    {
        DrawFolderTree("Assets");
        ImGui::TreePop();
    }
    ImGui::NextColumn();

    // �E�J���� = �I�𒆃t�H���_�̒��g
    DrawFileView();

    ImGui::Columns(1); // �J�����I��

    ImGui::End();

    ProcessPendingOperations(); // �폜�⃊�l�[��������
}

void ProjectPanel::DrawFolderTree(const fs::path& path)
{
    for (auto& entry : fs::directory_iterator(path))
    {
        if (!entry.is_directory()) continue; // �t�H���_�����\��

        const fs::path entryPath = entry.path();
        const std::string name = entryPath.filename().string();
        
        if (mRenaming && entryPath == mRenameTarget)
        {
            // === �t�H���_�����l�[�����[�h ===
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
                req.newStem = std::string(buffer); // �t�H���_�� stem �̑���ɐV�������O�����̂܂܎g��
                mRenameQueue.push_back(req);
                mRenaming = false;
            }

            // Esc �L�����Z��
            if (ImGui::IsItemDeactivated() && !ImGui::IsItemDeactivatedAfterEdit())
            {
                mRenaming = false;
            }

            ImGui::PopID();
        }
        else
        {
            // === �ʏ�̃t�H���_�\�� ===
            bool open = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);

            // ���N���b�N�őI�𒆃t�H���_���X�V
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                mCurrentFolder = entryPath;
            }

            // �E�N���b�N���j���[�iRename �I���� mRenaming �� true �ɂȂ�j
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

        // --- ���l�[���ΏۂȂ� InputText �ɐ؂�ւ� ---
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
                    // �t�H���_�͂��̂܂�
                    req.newStem = std::string(buffer);
                }
                else
                {
                    // �t�@�C���͊g���q���ێ�
                    req.newStem = std::string(buffer);
                }

                mRenameQueue.push_back(req);
                mRenaming = false;
            }

            // Esc �L�����Z��
            if (ImGui::IsItemDeactivated() && !ImGui::IsItemDeactivatedAfterEdit())
            {
                mRenaming = false;
            }

            ImGui::PopID();
        }
        else
        {
            // --- �ʏ�̕\�� ---
            if (ImGui::Selectable(name.c_str()))
            {
                if (entry.is_directory())
                {
                    // �t�H���_�N���b�N�ŊJ��
                    mCurrentFolder = entryPath;
                }
                else
                {
                    // �t�@�C���I��
                    g_SelectedFile = entryPath.string();
                }
            }

            // �R���e�L�X�g���j���[
			RightClickMenu(entryPath);


            // �h���b�O�J�n�����iSelectable �̋߂��ɒu���j
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
    // �R���e�L�X�g���j���[�͒��O�ɕ`�悵���A�C�e���iTreeNode �� Selectable�j�ɕR�Â�
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Open in External Editor"))
        {
            // Windows �̗�i������ SDL_OpenURL ���ɒu��������j
            std::string command = "start \"\" \"" + path.string() + "\"";
            system(command.c_str());
        }

        if (fs::is_directory(path))
        {
            if (ImGui::MenuItem("New Folder"))
            {
                // �ȈՓI�� NewFolder ����� (�Փ˂͍l�����Ă��Ȃ�)
                try { fs::create_directory(path / "NewFolder"); }
                catch (const std::exception& e) { Debug::Log("Create folder failed: %s\n", e.what()); }
            }
            if (ImGui::MenuItem("Delete Folder"))
            {
                // ���폜�͂��Ȃ��B�x���L���[�ɒǉ�����
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

        // Rename�i�t�H���_�E�t�@�C���ǂ���ł��j
        if (ImGui::MenuItem("Rename"))
        {
            mRenameTarget = path;
            // �t�@�C���Ȃ�g���q�������� stem ��ҏW�o�b�t�@�ɁA�t�H���_�� full name
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

    // �h���b�v��ɂ���
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_DRAG"))
        {
            const char* srcPathC = (const char*)payload->Data;
            fs::path src(srcPathC);
            fs::path dst = path / src.filename();

            // �������g��q���ւ̈ړ��͋֎~
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
    // �܂����l�[�����s���i���l�[����̖��O�Փ˃`�F�b�N���s���j
    for (const auto& req : mRenameQueue)
    {
        try
        {
            if (!fs::exists(req.oldPath)) continue; // ���ɏ����Ă�����X�L�b�v
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

    // ���ɍ폜����
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
