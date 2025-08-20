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
	static bool				isPlaying;
	static bool				isPaused;

	static Texture*			mPlayButtonTexture;
	static Texture*			mPauseButtonTexture;
	static Texture*			mStopButtonTexture;

	static class Renderer*	mRenderer;

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
	static bool IsPaused() { return isPaused; }

	static void SetRenderer(class Renderer* renderer) { mRenderer = renderer; }
};

