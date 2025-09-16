#include "ProjectPanel.h"

ProjectPanel::ProjectPanel(Renderer* renderer)
	:GUIPanel(renderer)
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
        ShowDirectoryRecursive("Assets");
	}
	ImGui::End();
}

void ProjectPanel::ShowDirectoryRecursive(const fs::path& path)
{
	// �w�肳�ꂽ�p�X���f�B���N�g���łȂ��ꍇ�͏I��
    // Assets�t�H���_���̃t�@�C����1������
    for (auto& entry : fs::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            // �t�H���_���c���[�m�[�h�Ƃ��ĕ\��
            if (ImGui::TreeNode(entry.path().filename().string().c_str()))
            {
                ShowDirectoryRecursive(entry.path()); // �ċA�Ăяo��
                ImGui::TreePop();
            }
        }
        else
        {
            // �t�@�C���\��
            if (ImGui::Selectable(entry.path().filename().string().c_str()))
            {
                g_SelectedFile = entry.path().string();
            }

            // �t�@�C���E�N���b�N���j���[
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Open in External Editor"))
                {
                    // ��: �O���G�f�B�^�ŊJ������ (OS�ˑ�)
                    printf("Open: %s\n", g_SelectedFile.c_str());
                }
                if (ImGui::MenuItem("Delete"))
                {
                    fs::remove(entry.path());
                    printf("Deleted: %s\n", entry.path().string().c_str());
                }
                ImGui::EndPopup();
            }
        }
    }
}
