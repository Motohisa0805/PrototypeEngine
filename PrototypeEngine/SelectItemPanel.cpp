#include "SelectItemPanel.h"

SelectItemPanel::SelectItemPanel(Renderer* renderer)
	:GUIPanel(renderer)
{
}

void SelectItemPanel::Draw(float width, float height)
{
	// �E�C���h�E�ʒu�ƃT�C�Y���Œ�
	ImGui::SetNextWindowPos(ImVec2(width * 0.8f, 30));
	ImGui::SetNextWindowSize(ImVec2((width * 0.2f), (float)height - 25));
	//  �V�����E�B���h�E�̍쐬
	ImGui::Begin("SelectItem", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{

	}
	ImGui::End();
}
