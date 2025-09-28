#include "GUIPanel.h"

GUIPanel::GUIPanel(Renderer* renderer)
	: mRenderer(renderer)
	, isMouseHovered(false)
	, isResetLayout(false)
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

void GUIPanel::GUIPanelMenu()
{
	// 2. ���O�̃A�C�e���ɑ΂���R���e�L�X�g���j���[�̒�`
	if (ImGui::BeginPopupContextWindow())
	{
		// �������|�b�v�A�b�v���j���[�̒��g
		// �|�b�v�A�b�v���J���Ă���Ԃ����`�悳��܂�

		if (ImGui::MenuItem("GUI Initialization of position"))
		{
			isResetLayout = true;
		}

		// 3. �|�b�v�A�b�v�̏I��
		ImGui::EndPopup();
	}
	if (isMouseHovered)
	{
	}
}

float GUIPanel::GetWindowSizeWidth()
{
	float width = WindowRenderProperty::GetWidth();

	return width;
}

float GUIPanel::GetWindowSizeHeight()
{
	float height = WindowRenderProperty::GetHeight();
	return height;
}

ImVec2 GUIPanel::GetAspectRatio()
{
	ImVec2 available_size = ImGui::GetContentRegionAvail();
	float W_a = available_size.x;
	float H_a = available_size.y;

	float W_d, H_d; // �`��T�C�Y

	// ������ɂ����ꍇ�̍���
	float H_test = W_a * (9.0f / 16.0f);

	if (H_test <= H_a) {
		// �c�����ɗ]�T�����邽�߁A�����ő���ɗ��p
		W_d = W_a;
		H_d = H_test;
	}
	else {
		// �������ɗ]�T�����邽�߁A�������ő���ɗ��p
		H_d = H_a;
		W_d = H_a * (16.0f / 9.0f);
	}

	// �`��T�C�Y���m�肵���̂ŁA�����񂹂̂��߂̃I�t�Z�b�g���v�Z

	// �I�t�Z�b�g�ʂ��v�Z
	float offset_x = (W_a - W_d) * 0.5f;
	float offset_y = (H_a - H_d) * 0.5f;

	// �J�[�\���ʒu���I�t�Z�b�g�������ړ�������
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset_x);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offset_y);

	return ImVec2(W_d,H_d);
}
