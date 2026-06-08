#include "GUIEditorManager.h"
#include "imgui_internal.h"
#include "EditorTextureManager.h"
#include "Renderer.h"
#include "WindowRenderProperty.h"
#include "GUIMainMenu.h"
#include "ToolbarPanel.h"
#include "GameViewPanel.h"
#include "SceneViewPanel.h"
#include "HierarchyPanel.h"
#include "ProjectPanel.h"
#include "InspectorPanel.h"

bool GUIEditorManager::isPaused = false;

bool GUIEditorManager::isPlaying = false;

bool GUIEditorManager::isStarting = false;

bool GUIEditorManager::isPushEnd = false;

bool GUIEditorManager::isFrameByFrame = false;

Renderer* GUIEditorManager::mRenderer = nullptr;


Vector2 GUIEditorManager::mSceneWinSize = Vector2::Zero;
	
ToolbarPanel* GUIEditorManager::mToolbarPanel = nullptr;

GUIMainMenu* GUIEditorManager::mGUIMainMenu = nullptr;

EditorWindow* GUIEditorManager::mRootMainWindow = nullptr;

bool GUIEditorManager::InitializeImGui(SDL_Window* window, SDL_GLContext glContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuiIOを参照
	ImGuiIO& io = ImGui::GetIO();
	//ドッキング機能の有効化
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui_ImplSDL3_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 330");
	//GUI用のフォントを読み込む
	//フォントパス(Libraryフォルダーにアクセス)
	string fontpath = "Library/Noto_Sans_JP/static/NotoSansJP-Bold.ttf";
	//新しいフォントを読み込み
	ImFont* font = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 20.0f);
	if (font)
	{
		io.FontDefault = font;
	}

	int windowWidth = WindowRenderProperty::GetWidth();
	int windowHeight = WindowRenderProperty::GetHeight();
	//タスクバーの生成
	mGUIMainMenu = new GUIMainMenu(mRenderer);
	mGUIMainMenu->Initialize(windowWidth, windowHeight);
	//ゲーム再生ボタンの生成
	mToolbarPanel = new ToolbarPanel(mRenderer);
	mToolbarPanel->Initialize(windowWidth, windowHeight);

	//パネルの事前定義
	RegisterAllEditorWindows(mRenderer);
	//ルートパネル追加
	mRootMainWindow = new EditorWindow(mRenderer);
	
	mRootMainWindow->AddEditorWindow(EditorWindowFactory::CreateEditorWindow("GameView"));
	mRootMainWindow->AddEditorWindow(EditorWindowFactory::CreateEditorWindow("SceneView"));
	mRootMainWindow->AddEditorWindow(EditorWindowFactory::CreateEditorWindow("Hierarchy"));
	mRootMainWindow->AddEditorWindow(EditorWindowFactory::CreateEditorWindow("Project"));
	mRootMainWindow->AddEditorWindow(EditorWindowFactory::CreateEditorWindow("Inspector"));

	SelectionManager::SetSelectedActor(nullptr);

	EditorTextureManager::GetInstance().AllLoad();

	return true;
}

void GUIEditorManager::UpdateImGuiState()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void GUIEditorManager::ResetPointer()
{
	for (auto window : mRootMainWindow->GetChildren()) {
		window->ClearPointer();
	}
}

void GUIEditorManager::RenderImGui()
{
	//エンジン全体に共通する入力処理
	if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_Z)) {
		CommandManager::Undo();
	}
	if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_Y)) {
		CommandManager::Redo();
	}

	// ここでImGuiの描画を行う
	// 画面サイズ（SDLで取得したウィンドウ幅/高さ）
	int windowWidth = WindowRenderProperty::GetWidth();
	int windowHeight = WindowRenderProperty::GetHeight();

	// 1. 画面全体を覆うためのフラグを設定（枠線やタイトルバー、移動を無効化）
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	// メインメニュー（GUIMainMenu）を表示するため、フルスクリーン化の計算を行う
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	// パネルの見た目を邪魔しない透明なウインドウを開始
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	// 背景を透明にする
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

	bool p_open = true;
	ImGui::Begin("MainDockSpaceWindow", &p_open, window_flags);

	ImGui::PopStyleVar(4); // StyleVar の復元
	ImGui::PopStyleColor(1);

	// 2. DockSpace 本体の生成
	ImGuiID dockspace_id = ImGui::GetID("MyEngineDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	/*
	// 初回起動時（または ini ファイルがない時）にデフォルト配置を組む
	static bool first_time = true;
	if (first_time)
	{
		first_time = false;

		// 既存の自動保存レイアウトを一度クリアして再構築する場合
		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

		// 画面を左右に分割 (左に 25% の領域を確保)
		ImGuiID dock_id_left;
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25f, nullptr, &dock_id_left);

		// 右側の領域をさらに上下に分割 (上に 60% の領域を確保)
		ImGuiID dock_id_right_top;
		ImGuiID dock_id_right_bottom = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Up, 0.60f, nullptr, &dock_id_right_top);

		//各ウィンドウの「ID（文字列）」を指定して、分割したスペースにドッキングさせる
		ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
		ImGui::DockBuilderDockWindow("GameView", dock_id_right_bottom);
		ImGui::DockBuilderDockWindow("SceneView", dock_id_right_top);
		ImGui::DockBuilderDockWindow("Inspector", dock_id_right); // 例：さらに右など

		ImGui::DockBuilderFinish(dockspace_id);
	}
	*/

	if (mGUIMainMenu) {
		mGUIMainMenu->Draw(windowWidth, windowHeight);
	}
	if (mToolbarPanel) {
		mToolbarPanel->Draw(windowWidth, windowHeight);
	}

	ImGui::End();


	//各パネルの描画
	for (auto window : mRootMainWindow->GetChildren()){
		if (window->IsShow()) {
			window->Draw(windowWidth, windowHeight);
		}
	}

	// 描画が終わった後、閉じられたウィンドウを非表示に
	// イテレータの破損を防ぐため、削除対象を一時的にリストアップして後から消す
	std::vector<EditorWindow*> toDelete;
	for (auto window : mRootMainWindow->GetChildren()) {
		if (!window->IsShow()) { // パネルが閉じられていたら
			toDelete.push_back(window);
		}
	}

	// リストアップしたウィンドウを安全に削除
	for (auto window : toDelete) {
		mRootMainWindow->RemoveEditorWindow(window);
	}

	//ファイル、フォルダ削除
	EditorSettingsManager::ProcessPendingDeletions();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIEditorManager::ShutdownImGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();


	if (mGUIMainMenu) {
		delete mGUIMainMenu;
		mGUIMainMenu = nullptr;
	}
	if (mToolbarPanel) {
		delete mToolbarPanel;
		mToolbarPanel = nullptr;
	}

	EditorWindowFactory::UnregisterAllEditorWindows();
	if (mRootMainWindow != nullptr) {
		delete mRootMainWindow;
		mRootMainWindow = nullptr;
	}
	
	if (SelectionManager::GetSelectedActor())
	{
		SelectionManager::SetSelectedActor(nullptr);
	}

	EditorTextureManager::GetInstance().AllRelease();
}
