#include "SelectItemPanel.h"

SelectItemPanel::SelectItemPanel(Renderer* renderer)
	:GUIPanel(renderer)
{
}

void SelectItemPanel::Initialize(float width, float height, ImTextureRef ref)
{	
	mWidthPos = width * 0.8f;
	mHeightPos = 55.0f;
	mWidthSize = width * 0.2f;
	mHeightSize = height - 55.0f;
}

void SelectItemPanel::Draw(float width, float height, ImTextureRef ref)
{
	// ウインドウ位置とサイズを固定
	ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize));
	//  新しいウィンドウの作成
	if(ImGui::Begin("SelectItem", nullptr, ImGuiWindowFlags_NoCollapse))
	{

	}
	ImGui::End();
}
