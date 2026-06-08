#include "ToolbarPanel.h"
#include "GUIEditorManager.h"

ToolbarPanel::ToolbarPanel(Renderer* renderer)
	:EditorWindow(renderer)
{
	mID = "Toolbar";
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
	EditorWindow::Initialize(width, height, ref);
	// ウインドウ位置とサイズを固定
	ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
}

void ToolbarPanel::ResetWindowPos(float width, float height)
{
}

void ToolbarPanel::Draw(float width, float height)
{
	ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
	if (ImGui::Begin(mID.c_str(),
		nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
	{
		BaseGUIPanelPopupMenu();
		ImGui::SetCursorPosX(width * 0.5f - 30); // 中央寄せ調整（60はボタン群の半幅）

		//再生/停止ボタン
		if (!GUIEditorManager::IsPlaying())
		{
			if (ImGui::ImageButton("PlayButton", (ImTextureID)(intptr_t)EditorTextureManager::GetInstance().GetPlayButtonTexture()->GetTextureID(), ImVec2(15, 15)))
			{
				GUIEditorManager::SetIsPlaying(true);
				GUIEditorManager::SetIsPaused(false);
				// スタートボタンが押された
				GUIEditorManager::SetIsStarting(true);
			}
		}
		else
		{
			if (ImGui::ImageButton("PlayButton", (ImTextureID)(intptr_t)EditorTextureManager::GetInstance().GetStopButtonTexture()->GetTextureID(), ImVec2(15, 15)))
			{
				GUIEditorManager::SetIsPlaying(false);
				GUIEditorManager::SetIsPaused(false);
				// 停止ボタンが押された
				GUIEditorManager::SetIsPushEnd(true);
			}
		}

		// 同じ行に Pause
		ImGui::SameLine();
		if (ImGui::ImageButton("PauseButton", (ImTextureID)(intptr_t)EditorTextureManager::GetInstance().GetPauseButtonTexture()->GetTextureID(), ImVec2(15, 15)))
		{
			if (GUIEditorManager::IsPlaying())
			{
				GUIEditorManager::SetIsPaused(!GUIEditorManager::IsPaused());
			}
		}

		if (GUIEditorManager::IsFrameByFrame())
		{
			GUIEditorManager::SetIsPaused(true);
			GUIEditorManager::SetIsFrameByFrame(false);
		}

		// 同じ行に FrameByFrame
		ImGui::SameLine();
		if (ImGui::ImageButton("FrameByFrameButton", (ImTextureID)(intptr_t)EditorTextureManager::GetInstance().GetFrameByFrameButtonTexture()->GetTextureID(), ImVec2(15, 15)))
		{
			if (GUIEditorManager::IsPlaying() && GUIEditorManager::IsPaused())
			{
				GUIEditorManager::SetIsFrameByFrame(true);
				GUIEditorManager::SetIsPaused(false);
			}
		}
	}
	ImGui::End();
}