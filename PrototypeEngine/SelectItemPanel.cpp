#include "SelectItemPanel.h"

SelectItemPanel::SelectItemPanel(Renderer* renderer)
	:GUIPanel(renderer)
{
}

void SelectItemPanel::Draw(float width, float height)
{
	// ウインドウ位置とサイズを固定
	ImGui::SetNextWindowPos(ImVec2(width * 0.8f, 30));
	ImGui::SetNextWindowSize(ImVec2((width * 0.2f), (float)height - 25));
	//  新しいウィンドウの作成
	ImGui::Begin("SelectItem", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{

	}
	ImGui::End();
}
