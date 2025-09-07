#include "GUIPanel.h"

GUIPanel::GUIPanel(Renderer* renderer)
	: mRenderer(renderer)
	, isMouseHovered(false)
{
}

bool GUIPanel::MouseHoveredDisble()
{
	isMouseHovered = false;
	return true;
}

bool GUIPanel::WindowHoveredConfirmation()
{
	//�}�E�X�����̃E�B���h�E�ɂ��邩�ǂ�������
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
	{
		// SceneView�p�l���Ƀ}�E�X������Ă���Ƃ��̏���
		isMouseHovered = true;
		return true;
	}
	return false;
}
