#pragma once
#include "SDOpenLib.h"
#include "SDL3.h"

#include "Texture.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

class GUIWinMain
{
private:
	static bool				mToggle;
	//�Đ������ǂ���
	static bool				isPlaying;
	//�X�^�[�g���������u��
	static bool				isStarting;
	//�I����Ă邩�ǂ���
	static bool				isPaused;
	//�I������u�Ԃ��ǂ���
	static bool				isPushEnd;

	static bool				isFrameByFrame;

	static Texture*			mPlayButtonTexture;
	static Texture*			mPauseButtonTexture;
	static Texture*			mStopButtonTexture;
	static Texture*			mFrameByFrameButtonTexture;

	static class Renderer*	mRenderer;


	//�Q�[���E�B���h�E�̃T�C�Y(��)
	static Vector2 			mGameWinPos;
	static Vector2 			mGameWinSize;
public:
	GUIWinMain() = default;
	~GUIWinMain() = default;
	// Initialize ImGui
	static bool InitializeImGui(SDL_Window* window, SDL_GLContext glContext);
	// Update ImGui state
	static void UpdateImGuiState();
	// Render ImGui
	static void RenderImGui();

	// Shutdown ImGui
	static void ShutdownImGui();

	static bool IsPlaying() { return isPlaying; }
	static void SetIsPlaying(bool playing) { isPlaying = playing; }
	static bool IsStarting() { return isStarting; }
	static void SetIsStarting(bool starting) { isStarting = starting; }
	static bool IsPaused() { return isPaused; }
	static void SetIsPaused(bool paused) { isPaused = paused; }
	static bool IsPushEnd() { return isPushEnd; }
	static void SetIsPushEnd(bool pushEnd) { isPushEnd = pushEnd; }

	static void SetRenderer(class Renderer* renderer) { mRenderer = renderer; }

	static Vector2 GetGameWinPos() { return mGameWinPos; }
	static Vector2 GetGameWinSize() { return mGameWinSize; }
};

