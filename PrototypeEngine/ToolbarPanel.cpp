#include "ToolbarPanel.h"

ToolbarPanel::ToolbarPanel(Renderer* renderer)
	:GUIPanel(renderer)
	, mPlayButtonTexture(nullptr)
	, mPauseButtonTexture(nullptr)
	, mStopButtonTexture(nullptr)
	, mFrameByFrameButtonTexture(nullptr)
{
}

ToolbarPanel::~ToolbarPanel()
{
	if (mPlayButtonTexture)
	{
		mPlayButtonTexture->Unload();
		delete mPlayButtonTexture;
		mPlayButtonTexture = nullptr;
	}
	if (mPauseButtonTexture)
	{
		mPauseButtonTexture->Unload();
		delete mPauseButtonTexture;
		mPauseButtonTexture = nullptr;
	}
	if (mStopButtonTexture)
	{
		mStopButtonTexture->Unload();
		delete mStopButtonTexture;
		mStopButtonTexture = nullptr;
	}
	if (mFrameByFrameButtonTexture)
	{
		mFrameByFrameButtonTexture->Unload();
		delete mFrameByFrameButtonTexture;
		mFrameByFrameButtonTexture = nullptr;
	}
}

void ToolbarPanel::Initialize(float width, float height, ImTextureRef ref)
{
	mWidthPos = 0.0f;
	mHeightPos = 25.0f; // メニューバーの下から開始
	mWidthSize = width;
	mHeightSize = 25.0f;

	// ツールバーは画面上部に固定
	mPlayButtonTexture = new Texture();
	if (!mPlayButtonTexture->Load("Assets/Editor/PlayButton.png"))
	{
		Debug::ErrorLog("Failed to load play button texture");
	}
	mStopButtonTexture = new Texture();
	if (!mStopButtonTexture->Load("Assets/Editor/StopButton.png"))
	{
		Debug::ErrorLog("Failed to load stop button texture");
	}
	mPauseButtonTexture = new Texture();
	if (!mPauseButtonTexture->Load("Assets/Editor/PauseButton.png"))
	{
		Debug::ErrorLog("Failed to load pause button texture");
	}
	mFrameByFrameButtonTexture = new Texture();
	if (!mFrameByFrameButtonTexture->Load("Assets/Editor/FrameByFrame.png"))
	{
		Debug::ErrorLog("Failed to load frame by frame button texture");
	}
}

void ToolbarPanel::ResetWindowPos(float width, float height)
{
}

void ToolbarPanel::Draw(float width, float height, ImTextureRef ref)
{

	// ウインドウ位置とサイズを固定
	ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos));
	ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize));

	if (ImGui::Begin(GetName(),
		nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
	{
		ImGui::SetCursorPosX(width * 0.5f - 30); // 中央寄せ調整（60はボタン群の半幅）

		//再生/停止ボタン
		if (!GUIWinMain::IsPlaying())
		{
			if (ImGui::ImageButton("PlayButton", (ImTextureID)(intptr_t)mPlayButtonTexture->GetTextureID(), ImVec2(15, 15)))
			{
				GUIWinMain::SetIsPlaying(true);
				GUIWinMain::SetIsPaused(false);
				// スタートボタンが押された
				GUIWinMain::SetIsStarting(true);
			}
		}
		else
		{
			if (ImGui::ImageButton("PlayButton", (ImTextureID)(intptr_t)mStopButtonTexture->GetTextureID(), ImVec2(15, 15)))
			{
				GUIWinMain::SetIsPlaying(false);
				GUIWinMain::SetIsPaused(false);
				// 停止ボタンが押された
				GUIWinMain::SetIsPushEnd(true);
			}
		}

		// 同じ行に Pause
		ImGui::SameLine();
		if (ImGui::ImageButton("PauseButton", (ImTextureID)(intptr_t)mPauseButtonTexture->GetTextureID(), ImVec2(15, 15)))
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
		if (ImGui::ImageButton("FrameByFrameButton", (ImTextureID)(intptr_t)mFrameByFrameButtonTexture->GetTextureID(), ImVec2(15, 15)))
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