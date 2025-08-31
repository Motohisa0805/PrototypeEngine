#include "HierarchyPanel.h"

HierarchyPanel::HierarchyPanel(Renderer* renderer)
	:mRenderer(renderer)
{
}

void HierarchyPanel::Draw(float width, float height)
{
	// ウインドウ位置とサイズを固定
	ImGui::SetNextWindowPos(ImVec2((width * 0.5f), 30));
	ImGui::SetNextWindowSize(ImVec2(width * 0.15f, (float)height - 25));
	//  新しいウィンドウの作成
	ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{

	}
	ImGui::End();
}