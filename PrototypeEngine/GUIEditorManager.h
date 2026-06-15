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
class SceneEditorCamera;
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
	static bool											mIsPlaying;
	//スタートを押した瞬間
	static bool											mIsStarting;
	//終わってるかどうか
	static bool											mIsPaused;
	//終わった瞬間かどうか
	static bool											mIsPushEnd;

	static bool											mIsFrameByFrame;

	static ToolbarPanel*								mToolbarPanel;
	
	static GUIMainMenu*									mGUIMainMenu;
	//エンジンの画面全体のパネルと見立てたWindow
	static EditorWindow*								mRootMainWindow;

	//パネル全体で数えるSceneEditorCamera配列
	static vector<SceneViewPanel*>						mSceneViewPanels;
public:
							GUIEditorManager() = default;
							~GUIEditorManager() = default;
	// Initialize ImGui
	static bool				 InitializeImGui(SDL_Window* window, SDL_GLContext glContext);
	static void				 InputUpdateImGuiState();
	// Update ImGui state	 
	static void				 UpdateImGuiState();
	//GUIパネルのゲームシーン内のポインターをリセット
	static void				 ResetPointer();
	// Render ImGui			 
	static void				 RenderImGui();

	static void				 SaveCurrentLayout(const char* filePath);

	static void				 LoadCustomLayout(const char* filePath);

	static void				 ApplyDefaultLayout_2by3();

	// Shutdown ImGui		 
	static void				 ShutdownImGui();
							 
	static bool				 IsPlaying() { return mIsPlaying; }
	static void				 SetIsPlaying(bool playing) { mIsPlaying = playing; }
	static bool				 IsStarting() { return mIsStarting; }
	static void				 SetIsStarting(bool starting) { mIsStarting = starting; }
	static bool				 IsPaused() { return mIsPaused; }
	static void				 SetIsPaused(bool paused) { mIsPaused = paused; }
	static bool				 IsPushEnd() { return mIsPushEnd; }
	static void				 SetIsPushEnd(bool pushEnd) { mIsPushEnd = pushEnd; }
	static bool				 IsFrameByFrame() { return mIsFrameByFrame; }
	static void				 SetIsFrameByFrame(bool frameByFrame) { mIsFrameByFrame = frameByFrame; }
	static void				 SetRenderer(class Renderer* renderer) { mRenderer = renderer; }
	static EditorWindow*	 GetRootMainWindow() { return mRootMainWindow; }

	static vector<SceneViewPanel*>		GetSceneViewPanels() { return mSceneViewPanels; }
	static void							AddSceneViewPanel(SceneViewPanel* panel);
	static void							RemoveSceneViewPanel(SceneViewPanel* panel);
};