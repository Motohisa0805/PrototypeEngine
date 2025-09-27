#include "GUIPanel.h"

GUIPanel::GUIPanel(Renderer* renderer)
	: mRenderer(renderer)
	, isMouseHovered(false)
{
}

void GUIPanel::Initialize(float width, float height, ImTextureRef ref)
{
}

bool GUIPanel::MouseHoveredDisble()
{
	isMouseHovered = false;
	return true;
}

void GUIPanel::ResetWindowPos(float width, float height)
{
}

bool GUIPanel::WindowHoveredConfirmation()
{
	//マウスがこのウィンドウにあるかどうか判定
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
	{
		// SceneViewパネルにマウスが乗っているときの処理
		isMouseHovered = true;
		return true;
	}
	return false;
}

void GUIPanel::Draw(float width, float height, ImTextureRef ref)
{

}
