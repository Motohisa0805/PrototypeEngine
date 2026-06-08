#pragma once
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "EditorWindowFactory.h"
#include "EditorWindow.h"
#include "CommandManager.h"

//前方宣言
//描画クラス
class Renderer;
//GUIパネルのベースクラス
class EditorWindow;
//メインメニュー
class GUIMainMenu;
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
class InspectorPanel;


class GUIEditorManager
{
private:
	//描画クラス
	static Renderer*									mRenderer;
	//***状態管理***
	//再生中かどうか
	static bool											isPlaying;
	//スタートを押した瞬間
	static bool											isStarting;
	//終わってるかどうか
	static bool											isPaused;
	//終わった瞬間かどうか
	static bool											isPushEnd;

	static bool											isFrameByFrame;

	static Vector2 										mSceneWinSize;

	static ToolbarPanel*								mToolbarPanel;
	
	static GUIMainMenu*									mGUIMainMenu;
	//エンジンの画面全体のパネルと見立てたWindow
	static EditorWindow*								mRootMainWindow;
public:
							GUIEditorManager() = default;
							~GUIEditorManager() = default;
	// Initialize ImGui
	static bool				 InitializeImGui(SDL_Window* window, SDL_GLContext glContext);
	// Update ImGui state	 
	static void				 UpdateImGuiState();
	//GUIパネルのゲームシーン内のポインターをリセット
	static void				 ResetPointer();
	// Render ImGui			 
	static void				 RenderImGui();
	// Shutdown ImGui		 
	static void				 ShutdownImGui();
							 
	static bool				 IsPlaying() { return isPlaying; }
	static void				 SetIsPlaying(bool playing) { isPlaying = playing; }
	static bool				 IsStarting() { return isStarting; }
	static void				 SetIsStarting(bool starting) { isStarting = starting; }
	static bool				 IsPaused() { return isPaused; }
	static void				 SetIsPaused(bool paused) { isPaused = paused; }
	static bool				 IsPushEnd() { return isPushEnd; }
	static void				 SetIsPushEnd(bool pushEnd) { isPushEnd = pushEnd; }
	static bool				 IsFrameByFrame() { return isFrameByFrame; }
	static void				 SetIsFrameByFrame(bool frameByFrame) { isFrameByFrame = frameByFrame; }
							 
	static void				 SetRenderer(class Renderer* renderer) { mRenderer = renderer; }

	static Vector2			 GetSceneWinSize() { return mSceneWinSize; }
	static void				 SetSceneWinSize(const Vector2& size) { mSceneWinSize = size; }

	static EditorWindow*	 GetRootMainWindow() { return mRootMainWindow; }
};