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
	//�}�E�X�����̃E�B���h�E�ɂ��邩�ǂ�������
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
	{
		// SceneView�p�l���Ƀ}�E�X������Ă���Ƃ��̏���
		isMouseHovered = true;
		return true;
	}
	return false;
}

void GUIPanel::Draw(float width, float height, ImTextureRef ref)
{

}
