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
	// ツールバーは画面上部に固定
	mWidthPos = 0.0f;
	mHeightPos = 25.0f; // メニューバーの下から開始
	mWidthSize = width;
	mHeightSize = 25.0f;

	
}

void ToolbarPanel::ResetWindowPos(float width, float height)
{
}

void ToolbarPanel::Draw(float width, float height, ImTextureRef ref)
{

	// ウインドウ位置とサイズを固定
	ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);

	if (ImGui::Begin(GetName(),
		nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
	{
		GUIPanelMenu();
		ImGui::SetCursorPosX(width * 0.5f - 30); // 中央寄せ調整（60はボタン群の半幅）

		//再生/停止ボタン
		if (!GUIWinMain::IsPlaying())
		{
			if (ImGui::ImageButton("PlayButton", (ImTextureID)(intptr_t)EditorTextureManager::GetInstance().GetPlayButtonTexture()->GetTextureID(), ImVec2(15, 15)))
			{
				GUIWinMain::SetIsPlaying(true);
				GUIWinMain::SetIsPaused(false);
				// スタートボタンが押された
				GUIWinMain::SetIsStarting(true);
			}
		}
		else
		{
			if (ImGui::ImageButton("PlayButton", (ImTextureID)(intptr_t)EditorTextureManager::GetInstance().GetStopButtonTexture()->GetTextureID(), ImVec2(15, 15)))
			{
				GUIWinMain::SetIsPlaying(false);
				GUIWinMain::SetIsPaused(false);
				// 停止ボタンが押された
				GUIWinMain::SetIsPushEnd(true);
			}
		}

		// 同じ行に Pause
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

		// 同じ行に FrameByFrame
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