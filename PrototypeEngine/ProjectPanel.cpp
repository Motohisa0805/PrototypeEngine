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
        ImGui::Columns(2); // 2�J�����ɕ���

        // ���J���� = �t�H���_�c���[
        if (ImGui::TreeNode("Assets"))
        {
            // ���N���b�N�őI�𒆃t�H���_���X�V
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
    ProcessPendingOperations(); // �폜�⃊�l�[��������

	// �h���b�O�I��
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
        // �����ɁuImport New Asset�v�Ȃǂ��ǉ��ł���
        ImGui::EndPopup();
    }
    return true;
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
                mCurrentFolder = entryPath;
                mSelectedPath = entryPath; // �I���p�X���X�V
            }

            // �ʏ�̃t�H���_�p���j���[�i�폜�E���l�[���j
            // �E�N���b�N���j���[�iRename �I���� mRenaming �� true �ɂȂ�j
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

    // === �p���������X�g�\�� ===
    fs::path root = "Assets";
    fs::path relative = fs::relative(mCurrentFolder, root);

    // ���[�g ("Assets") ��K���\��
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

    ImGui::Separator(); // �p�������ƃt�@�C�����X�g����؂�

    for (auto& entry : fs::directory_iterator(mCurrentFolder))
    {
        DrawFileSystemEntry(entry);
    }
}

void ProjectPanel::DrawFileSystemEntry(const fs::directory_entry& entry)
{
    const fs::path entryPath = entry.path();
    const std::string name = entryPath.filename().string();

    // ���l�[������
    if (mRenaming && entryPath == mRenameTarget)
    {
        RenameFunction(entry);
        return; // ���l�[�����͈ȍ~�̏������s��Ȃ�
    }

    // -- - �ʏ�̕\��-- -
    ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_None;
    if (entryPath == mSelectedPath)
    {
        selectableFlags |= ImGuiSelectableFlags_Highlight; // �n�C���C�g�\��
    }

    // �t�H���_�E�t�@�C���I�� (�P��N���b�N�ł̑I���ƈړ�)
    // ���̏����ŁA�t�H���_�ł��t�@�C���ł� mSelectedPath �͍X�V�����
    if (ImGui::Selectable(name.c_str(), false, selectableFlags))
    {
        mSelectedPath = entryPath;

        // �V���O���N���b�N�Ńt�H���_�ړ����������Ȃ��ꍇ�͂��̃u���b�N���폜
        if (!entry.is_directory())
        {
            // �t�@�C���I��
            mCurrentFile = entryPath.string();
        }
    }

    // �_�u���N���b�N����
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        if (entry.is_directory())
        {
            // �t�H���_�̏ꍇ�݈̂ړ������s
            mCurrentFolder = entryPath;

            // �_�u���N���b�N�ňړ������ꍇ�A�I����Ԃ��X�V����
            mSelectedPath = entryPath;
        }
        else
        {
            // �t�@�C���̏ꍇ�i�O���G�f�B�^�ŊJ���Ȃǂ̏����������ɒǉ��j
        }
    }

    // �R���e�L�X�g���j���[�A�V���[�g�J�b�g�A�h���b�O���h���b�v
    RightClickMenu(entryPath);
    ShortcutKeyInputFunction(entryPath); // ����������path��n���悤�ɕύX
    DragDropFunction(entryPath);
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
                try
                {
                    // �ȈՓI�ȃ��j�[�N�������̗�
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

void ProjectPanel::ShortcutKeyInputFunction(const fs::path& path)
{
    //�폜�L�[
    if (!mSelectedPath.empty() && ImGui::IsKeyPressed(ImGuiKey_Delete))
    {
        mDeleteQueue.push_back(mSelectedPath); //mSelectedPath ���g�p
    }
    //���O�ύX�L�[
    if (!mSelectedPath.empty() && ImGui::IsKeyPressed(ImGuiKey_F2))
    {
        mRenameTarget = mSelectedPath; //mSelectedPath ���^�[�Q�b�g��

        // �t�@�C���Ȃ�g���q�������� stem ��ҏW�o�b�t�@�ɁA�t�H���_�� full name
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
        // �h���b�O�J�n�����iSelectable �̋߂��ɒu���j
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
                    if (fs::exists(dst))
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

void ProjectPanel::DrawOverwritePopup()
{
    if (ImGui::BeginPopupModal("Overwrite?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("File already exists:\n%s\n\nOverwrite?", mPendingDst.string().c_str());

        if (ImGui::Button("Yes", ImVec2(120, 0)))
        {
            try
            {
                fs::remove(mPendingDst);            // ����������
                fs::rename(mPendingSrc, mPendingDst); // �ړ�
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
