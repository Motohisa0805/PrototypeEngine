#include "HierarchyPanel.h"

HierarchyPanel::HierarchyPanel(Renderer* renderer)
	:mRenderer(renderer)
{
}

void HierarchyPanel::Draw(float width, float height)
{
	// �E�C���h�E�ʒu�ƃT�C�Y���Œ�
	ImGui::SetNextWindowPos(ImVec2((width * 0.5f), 30));
	ImGui::SetNextWindowSize(ImVec2(width * 0.15f, (float)height - 25));
	//  �V�����E�B���h�E�̍쐬
	ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{

	}
	ImGui::End();
}