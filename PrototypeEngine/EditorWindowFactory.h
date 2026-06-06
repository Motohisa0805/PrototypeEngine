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

class EditorWindowFactory
{
private:
	//EditorWindow名と、それを生成する関数のマップ
	static std::unordered_map<string, EditorWindow*>	sCreators;
public:
	//ファクトリーの登録処理
	static void					RegisterEditorWindow(EditorWindow* creator);

	static EditorWindow*		CreateEditorWindow(const string& type);

	//登録されているコンポーネントの一覧を取得
	static std::vector<string>	GetRegisteredEditorWindowNames();
	static void					UnregisterAllEditorWindows();
};

extern void RegisterAllEditorWindows(Renderer* renderer);
