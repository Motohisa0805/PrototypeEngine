#include "ProjectPanel.h"

ProjectPanel::ProjectPanel(Renderer* renderer)
	:GUIPanel(renderer)
{
}

void ProjectPanel::Draw(float width, float height)
{
	// ウインドウ位置とサイズを固定
	ImGui::SetNextWindowPos(ImVec2(width * 0.65f, 30));
	ImGui::SetNextWindowSize(ImVec2(width * 0.15f, (float)height - 25));
	//  新しいウィンドウの作成
	ImGui::Begin("Project", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{

	}
	ImGui::End();
}
