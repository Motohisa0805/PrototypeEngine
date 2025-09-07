#pragma once
#include "SDL3.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

//前方宣言
//描画クラス
class Renderer;
//ツールバーのパネル
class ToolbarPanel;
//シーンビューのパネル
class SceneViewPanel;
//ゲームビューのパネル
class GameViewPanel;
//ヒエラルキーパネル
class HierarchyPanel;
//プロジェクト選択用のパネル
class ProjectPanel;
//アイテム選択用のパネル
class SelectItemPanel;

class GUIWinMain
{
private:
	//描画クラス
	static Renderer*				mRenderer;

	//***状態管理***
	//再生中かどうか
	static bool						isPlaying;
	//スタートを押した瞬間
	static bool						isStarting;
	//終わってるかどうか
	static bool						isPaused;
	//終わった瞬間かどうか
	static bool						isPushEnd;

	static bool						isFrameByFrame;

	//ゲームウィンドウのサイズ(横)
	static Vector2 					mGameWinPos;
	//ゲームウィンドウのサイズ(横)
	static Vector2 					mGameWinSize;

	static Vector2 					mSceneWinSize;

	//ツールバーのパネル
	static ToolbarPanel*			mToolbarPanel;
	//シーンビューのパネル
	static SceneViewPanel*			mSceneViewPanel;
	//ゲームビューのパネル
	static GameViewPanel*			mGameViewPanel;
	//ヒエラルキーパネル
	static HierarchyPanel*			mHierarchyPanel;
	//プロジェクト選択用のパネル
	static ProjectPanel*			mProjectPanel;
	//アイテム選択用のパネル
	static SelectItemPanel*			mSelectItemPanel;

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
	static bool IsFrameByFrame() { return isFrameByFrame; }
	static void SetIsFrameByFrame(bool frameByFrame) { isFrameByFrame = frameByFrame; }

	static void SetRenderer(class Renderer* renderer) { mRenderer = renderer; }

	static Vector2 GetGameWinPos() { return mGameWinPos; }
	static void SetGameWinPos(const Vector2& pos) { mGameWinPos = pos; }
	static Vector2 GetGameWinSize() { return mGameWinSize; }
	static void SetGameWinSize(const Vector2& size) { mGameWinSize = size; }

	static Vector2 GetSceneWinSize() { return mSceneWinSize; }
	static void SetSceneWinSize(const Vector2& size) { mSceneWinSize = size; }

	static class ToolbarPanel* GetToolbarPanel() { return mToolbarPanel; }
	static class SceneViewPanel* GetSceneViewPanel() { return mSceneViewPanel; }
	static class GameViewPanel* GetGameViewPanel() { return mGameViewPanel; }
	static class HierarchyPanel* GetHierarchyPanel() { return mHierarchyPanel; }
	static class ProjectPanel* GetProjectPanel() { return mProjectPanel; }
	static class SelectItemPanel* GetSelectItemPanel() { return mSelectItemPanel; }
};

