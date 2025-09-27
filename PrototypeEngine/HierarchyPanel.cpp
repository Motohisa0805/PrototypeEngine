#include "HierarchyPanel.h"

HierarchyPanel::HierarchyPanel(Renderer* renderer)
	:GUIPanel(renderer)
{
}

void HierarchyPanel::Initialize(float width, float height, ImTextureRef ref)
{
	mWidthPos = width * 0.5f;
	mHeightPos = 55.0f;
	mWidthSize = width * 0.15f;
	mHeightSize = height - 55.0f;
}

void HierarchyPanel::Draw(float width, float height, ImTextureRef ref)
{
	// �E�C���h�E�ʒu�ƃT�C�Y���Œ�
	ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize));
	//  �V�����E�B���h�E�̍쐬
	if(ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse))
	{

	}
	ImGui::End();
}