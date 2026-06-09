#pragma once
#include "Typedefs.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
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

using WindowCreator = std::function<EditorWindow* (Renderer*)>;

class EditorWindowFactory
{
private:
	//EditorWindow名と、それを生成する関数のマップ
	static std::unordered_map<string, WindowCreator>	sCreators;

	static std::unordered_map<string, int>				sInstanceCounters;

	static Renderer*									mRenderer;
public:
	//ファクトリーの登録処理
	static void					RegisterEditorWindow(const string& id, WindowCreator creator);

	static EditorWindow*		CreateEditorWindow(const string& type, Renderer* renderer);

	//登録されているコンポーネントの一覧を取得
	static std::vector<string>	GetRegisteredEditorWindowNames();
	static void					UnregisterAllEditorWindows();
};

extern void RegisterAllEditorWindows();
