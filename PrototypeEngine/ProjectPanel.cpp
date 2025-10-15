#include "ProjectPanel.h"
#include "EditorSettingsManager.h"

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
        ImGui::Columns(2); // 2�J�����ɕ���

        // ���J���� = �t�H���_�c���[
        if (ImGui::TreeNode("Assets"))
        {
            // ���N���b�N�őI�𒆃t�H���_���X�V(Assets�t�H���_�p)
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                mCurrentFolder = "Assets";
            }
            AssetsFolderPrivateMenu();
            DrawFolderTree("Assets");
            ImGui::TreePop();
        }
        ImGui::NextColumn();
    
        // �E�J���� = �I�𒆃t�H���_�̒��g
        DrawFileView();

        // �J�����I��
        ImGui::Columns(1); 
    }
    ImGui::End();

    DrawOverwritePopup();
    // �폜�⃊�l�[��������
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
        // �����ɁuImport New Asset�v�Ȃǂ��ǉ��ł���
        ImGui::EndPopup();
    }
    return true;
}

void ProjectPanel::DrawFolderTree(const filesystem::path& path)
{
    for (auto& entry : filesystem::directory_iterator(path))
    {
        if (!entry.is_directory()) continue; // �t�H���_�����\��

        const string name = entry.path().filename().string();
        
        //���O�ύX����
        if (mRenaming && entry.path() == mPathToRename)
        {
            RenameFunction(entry);
        }
        else
        {
            // �c���[�m�[�h�̕\��
            // ImGuiTreeNodeFlags_Selected: mSelectedPath�ƈ�v����ꍇ�Ƀn�C���C�g�\��������
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
            if (path == mSelectedPath)
            {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            bool open = ImGui::TreeNodeEx(name.c_str(), flags); // �C��: flags���g�p

            // ���N���b�N�őI�𒆃t�H���_���X�V
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                mCurrentFolder = entry.path();
                mSelectedPath = entry.path(); // �I���p�X���X�V
            }

            // �ʏ�̃t�H���_�p���j���[�i�폜�E���l�[���j
            // �E�N���b�N���j���[�iRename �I���� mRenaming �� true �ɂȂ�j
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

void ProjectPanel::DrawFileView()
{
    if (!filesystem::exists(mCurrentFolder)) return;

    // === �p���������X�g�\�� ===
    filesystem::path root = "Assets";
    filesystem::path relative = filesystem::relative(mCurrentFolder, root);

    // ���[�g ("Assets") ��K���\��
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

    ImGui::Separator(); // �p�������ƃt�@�C�����X�g����؂�

    for (auto& entry : filesystem::directory_iterator(mCurrentFolder))
    {
        DrawFileSystemEntry(entry);
    }
}

void ProjectPanel::DrawFileSystemEntry(const filesystem::directory_entry& entry)
{
    const string name = entry.path().filename().string();

    // ���l�[������
    if (mRenaming && entry.path() == mPathToRename)
    {
        RenameFunction(entry);
        return; // ���l�[�����͈ȍ~�̏������s��Ȃ�
    }

    // -- - �ʏ�̕\��-- -
    ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_None;
    if (entry.path() == mSelectedPath)
    {
        selectableFlags |= ImGuiSelectableFlags_Highlight; // �n�C���C�g�\��
    }

    // �t�H���_�E�t�@�C���I�� (�P��N���b�N�ł̑I���ƈړ�)
    // ���̏����ŁA�t�H���_�ł��t�@�C���ł� mSelectedPath �͍X�V�����
    if (ImGui::Selectable(name.c_str(), false, selectableFlags))
    {
        mSelectedPath = entry.path();

        // �V���O���N���b�N�Ńt�H���_�ړ����������Ȃ��ꍇ�͂��̃u���b�N���폜
        if (!entry.is_directory())
        {
            // �t�@�C���I��
            mCurrentFile = entry.path().string();
        }
    }

    // �_�u���N���b�N����
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        if (entry.is_directory())
        {
            // �t�H���_�̏ꍇ�݈̂ړ������s
            mCurrentFolder = entry.path();

            // �_�u���N���b�N�ňړ������ꍇ�A�I����Ԃ��X�V����
            mSelectedPath = entry.path();
        }
        else
        {
            // �t�@�C���̏ꍇ
            if (entry.path().extension().string() == ".json")
            {
                // �V�[���t�@�C���̃��[�h�������Ăяo��
                // ���s���̃V�[���Ɛ؂�ւ��邽�߁ASceneManager�ɏ������˗����܂�
                SceneManager::LoadSceneFromFile(entry.path().string());
                EditorSettingsManager::GetInstance().SetLastOpenedScene(entry.path().string());
            }
            else
            {
                // ���̑��̃t�@�C���̏ꍇ�i�O���G�f�B�^�ŊJ���Ȃǁj
            }
        }
    }
    //�E�N���b�N����
    RightClickMenu(entry.path());
    // �R���e�L�X�g���j���[�A�V���[�g�J�b�g�A�h���b�O���h���b�v
    ShortcutKeyInputFunction(entry.path()); 
    DragDropFunction(entry.path());
}

bool ProjectPanel::RightClickMenu(const filesystem::path& path)
{
    // �R���e�L�X�g���j���[�͒��O�ɕ`�悵���A�C�e���iTreeNode �� Selectable�j�ɕR�Â�
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Open in External Editor"))
        {
            // Windows �̗�i������ SDL_OpenURL ���ɒu��������j
            string command = "start \"\" \"" + path.string() + "\"";
            system(command.c_str());
        }

        if (filesystem::is_directory(path))
        {
            if (ImGui::MenuItem("New Folder"))
            {
                // �ȈՓI�� NewFolder ����� (�Փ˂͍l�����Ă��Ȃ�)
                try
                {
                    // �ȈՓI�ȃ��j�[�N�������̗�
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

            //�V�[���쐬
            if (ImGui::MenuItem("New Scene"))
            {
                string uniqueName = "NewScene.json"; // �g���q�t���ŏ�����
                filesystem::path targetFolder = path; // ���݉E�N���b�N���Ă���p�X�i�t�H���_�j

                // ���ɑ��݂���t�@�C�������`�F�b�N���A���j�[�N�Ȗ��O�ɕύX����
                int counter = 1;
                while (filesystem::exists(targetFolder / uniqueName)) {
                    // NewScene(1).json, NewScene(2).json �̂悤�ɐ���
                    uniqueName = "NewScene (" + std::to_string(counter++) + ").json";
                }

                filesystem::path newScenePath = targetFolder / uniqueName;

                // 3. SceneSerializer���g���ċ�̃V�[���f�[�^���t�@�C���ɏ����o��
                // SceneSerializer::SaveEmptyScene()���Ńt�@�C���������ݏ������s��
                if (SceneSerializer::SaveEmptyScene(newScenePath))
                {
                    // �������O
                    Debug::Log("Created new scene: %s\n", newScenePath.string().c_str());
                }
                else
                {
                    // ���s���O
                    Debug::Log("Failed to create scene file: %s\n", newScenePath.string().c_str());
                }
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
            mPathToRename = path;
            // �t�@�C���Ȃ�g���q�������� stem ��ҏW�o�b�t�@�ɁA�t�H���_�� full name
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

        ImGui::EndPopup();
    }

    return false;
}

void ProjectPanel::ShortcutKeyInputFunction(const filesystem::path& path)
{
    //�폜�L�[
    if (!mSelectedPath.empty() && ImGui::IsKeyPressed(ImGuiKey_Delete))
    {
        mDeleteQueue.push_back(mSelectedPath); //mSelectedPath ���g�p
    }
    //���O�ύX�L�[
    if (!mSelectedPath.empty() && ImGui::IsKeyPressed(ImGuiKey_F2))
    {
        mPathToRename = mSelectedPath; //mSelectedPath ���^�[�Q�b�g��

        // �t�@�C���Ȃ�g���q�������� stem ��ҏW�o�b�t�@�ɁA�t�H���_�� full name
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
    // �h���b�O�J�n�����iSelectable �̋߂��ɒu���j
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", filePath.c_str(), filePath.size() + 1);
        // �h���b�O���̕\��
        ImGui::Text("%s", path.filename().string().c_str());
        ImGui::EndDragDropSource();
    }
    //�h���b�O�I������
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
        {
            const char* srcPathC = (const char*)payload->Data;
            filesystem::path src(srcPathC);
            filesystem::path dst;

            if (filesystem::is_directory(path))
            {
                // �t�H���_�Ƀh���b�v �� ���ɓ����
                dst = path / src.filename();
            }
            else
            {
                // �t�@�C���Ƀh���b�v �� �����K�w
                dst = path.parent_path() / src.filename();
            }

            if (src != dst && !src.string().starts_with(dst.string()))
            {
                if (filesystem::exists(dst))
                {
                    // ���łɑ��� �� �㏑���m�F�_�C�A���O��
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
            // �t�H���_�͂��̂܂�
            req.newStem = string(buffer);
        }
        else
        {
            // �t�@�C���͊g���q���ێ�
            req.newStem = string(buffer);
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

void ProjectPanel::DrawOverwritePopup()
{
    if (ImGui::BeginPopupModal("Overwrite?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("File already exists:\n%s\n\nOverwrite?", mPendingDst.string().c_str());

        if (ImGui::Button("Yes", ImVec2(120, 0)))
        {
            try
            {
                filesystem::remove(mPendingDst);            // ����������
                filesystem::rename(mPendingSrc, mPendingDst); // �ړ�
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
    // �܂����l�[�����s���i���l�[����̖��O�Փ˃`�F�b�N���s���j
    for (const auto& req : mRenameQueue)
    {
        try
        {
            if (!filesystem::exists(req.oldPath)) continue; // ���ɏ����Ă�����X�L�b�v
            filesystem::path newPath = req.oldPath.parent_path() / (req.newStem + req.oldPath.extension().string());
            if (filesystem::exists(newPath))
            {
                Debug::Log("Rename failed: %s already exists\n", newPath.string().c_str());
            }
            else
            {
                filesystem::rename(req.oldPath, newPath);
                Debug::Log("Renamed: %s -> %s\n", req.oldPath.string().c_str(), newPath.string().c_str());
            }
        }
        catch (const exception& e)
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
            if (!filesystem::exists(p)) continue;
            if (filesystem::is_directory(p))
            {
                filesystem::remove_all(p);
                Debug::Log("Deleted folder: %s\n", p.string().c_str());
            }
            else
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
