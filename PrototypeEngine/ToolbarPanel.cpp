#include "ToolbarPanel.h"

ToolbarPanel::ToolbarPanel(Renderer* renderer)
	:GUIPanel(renderer)
{
}

ToolbarPanel::~ToolbarPanel()
{
}

void ToolbarPanel::Initialize(float width, float height, ImTextureRef ref)
{
	// �c�[���o�[�͉�ʏ㕔�ɌŒ�
	mWidthPos = 0.0f;
	mHeightPos = 25.0f; // ���j���[�o�[�̉�����J�n
	mWidthSize = width;
	mHeightSize = 25.0f;

	
}

void ToolbarPanel::ResetWindowPos(float width, float height)
{
}

void ToolbarPanel::Draw(float width, float height, ImTextureRef ref)
{

	// �E�C���h�E�ʒu�ƃT�C�Y���Œ�
	ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);

	if (ImGui::Begin(GetName(),
		nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
	{
		GUIPanelMenu();
		ImGui::SetCursorPosX(width * 0.5f - 30); // �����񂹒����i60�̓{�^���Q�̔����j

		//�Đ�/��~�{�^��
		if (!GUIWinMain::IsPlaying())
		{
			if (ImGui::ImageButton("PlayButton", (ImTextureID)(intptr_t)EditorTextureManager::GetInstance().GetPlayButtonTexture()->GetTextureID(), ImVec2(15, 15)))
			{
				GUIWinMain::SetIsPlaying(true);
				GUIWinMain::SetIsPaused(false);
				// �X�^�[�g�{�^���������ꂽ
				GUIWinMain::SetIsStarting(true);
			}
		}
		else
		{
			if (ImGui::ImageButton("PlayButton", (ImTextureID)(intptr_t)EditorTextureManager::GetInstance().GetStopButtonTexture()->GetTextureID(), ImVec2(15, 15)))
			{
				GUIWinMain::SetIsPlaying(false);
				GUIWinMain::SetIsPaused(false);
				// ��~�{�^���������ꂽ
				GUIWinMain::SetIsPushEnd(true);
			}
		}

		// �����s�� Pause
		ImGui::SameLine();
		if (ImGui::ImageButton("PauseButton", (ImTextureID)(intptr_t)EditorTextureManager::GetInstance().GetPauseButtonTexture()->GetTextureID(), ImVec2(15, 15)))
		{
			if (GUIWinMain::IsPlaying())
			{
				GUIWinMain::SetIsPaused(!GUIWinMain::IsPaused());
			}
		}

		if (GUIWinMain::IsFrameByFrame())
		{
			GUIWinMain::SetIsPaused(true);
			GUIWinMain::SetIsFrameByFrame(false);
		}

		// �����s�� FrameByFrame
		ImGui::SameLine();
		if (ImGui::ImageButton("FrameByFrameButton", (ImTextureID)(intptr_t)EditorTextureManager::GetInstance().GetFrameByFrameButtonTexture()->GetTextureID(), ImVec2(15, 15)))
		{
			if (GUIWinMain::IsPlaying() && GUIWinMain::IsPaused())
			{
				GUIWinMain::SetIsFrameByFrame(true);
				GUIWinMain::SetIsPaused(false);
			}
		}
	}
	ImGui::End();
}