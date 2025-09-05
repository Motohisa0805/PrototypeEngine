#include "GUIPanel.h"

GUIPanel::GUIPanel()
	:isMouseHovered(false)
{
}

bool GUIPanel::MouseHoveredDisble()
{
	isMouseHovered = false;
	return true;
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
