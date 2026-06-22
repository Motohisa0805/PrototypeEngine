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
#include "SceneEditorCamera.h"
#include "DebugManager.h"

bool GUIEditorManager::mIsPaused = false;

bool GUIEditorManager::mIsPlaying = false;

bool GUIEditorManager::mIsStarting = false;

bool GUIEditorManager::mIsPushEnd = false;

bool GUIEditorManager::mIsFrameByFrame = false;

Renderer* GUIEditorManager::mRenderer = nullptr;
	
ToolbarPanel* GUIEditorManager::mToolbarPanel = nullptr;

GUIMainMenu* GUIEditorManager::mGUIMainMenu = nullptr;

EditorWindow* GUIEditorManager::mRootMainWindow = nullptr;

vector<SceneViewPanel*> GUIEditorManager::mSceneViewPanels;

bool GUIEditorManager::InitializeImGui(SDL_Window* window, SDL_GLContext glContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuiIOを参照
	ImGuiIO& io = ImGui::GetIO();
	// フォントの設定オブジェクトを作成
	ImFontConfig config;
	config.MergeMode = false;// 他のフォントと結合しない
	//日本語の文字コード範囲を取得
	const ImWchar* glyphRanges = io.Fonts->GetGlyphRangesJapanese();

	//ドッキング機能の有効化
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui_ImplSDL3_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 330");
	//GUI用のフォントを読み込む
	//フォントパス(Libraryフォルダーにアクセス)
	string fontpath = "Library/Noto_Sans_JP/static/NotoSansJP-Bold.ttf";
	//新しいフォントを読み込み
	ImFont* font = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 20.0f,&config,glyphRanges);
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
	RegisterAllEditorWindows();
	//ルートパネル追加
	mRootMainWindow = new EditorWindow(mRenderer);
	
	mRootMainWindow->AddEditorWindow(EditorWindowFactory::CreateEditorWindow("GameView", mRenderer));
	mRootMainWindow->AddEditorWindow(EditorWindowFactory::CreateEditorWindow("SceneView", mRenderer));
	mRootMainWindow->AddEditorWindow(EditorWindowFactory::CreateEditorWindow("Hierarchy", mRenderer));
	mRootMainWindow->AddEditorWindow(EditorWindowFactory::CreateEditorWindow("Project", mRenderer));
	mRootMainWindow->AddEditorWindow(EditorWindowFactory::CreateEditorWindow("Inspector", mRenderer));

	SelectionManager::SetSelectedActor(nullptr);

	EditorTextureManager::GetInstance().AllLoad();

	return true;
}

void GUIEditorManager::InputUpdateImGuiState()
{
	for (auto window : mRootMainWindow->GetChildren()) {
		window->InputUpdate();
	}
}

void GUIEditorManager::UpdateImGuiState()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	for (auto window : mRootMainWindow->GetChildren()) {
		window->Update();
	}
}

void GUIEditorManager::ResetPointer()
{
	for (auto window : mRootMainWindow->GetChildren()) {
		window->ClearPointer();
	}
}

void GUIEditorManager::RenderImGui()
{
	// ここでImGuiの描画を行う
	// 画面サイズ（SDLで取得したウィンドウ幅/高さ）
	int windowWidth = WindowRenderProperty::GetWidth();
	int windowHeight = WindowRenderProperty::GetHeight();

	if (mGUIMainMenu) {
		mGUIMainMenu->Draw(windowWidth, windowHeight);
	}
	if (mToolbarPanel) {
		mToolbarPanel->Draw(windowWidth, windowHeight);
	}

	//エンジン全体に共通する入力処理
	if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_Z)) {
		CommandManager::Undo();
	}
	if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_Y)) {
		CommandManager::Redo();
	}


	// 1. 画面全体を覆うためのフラグを設定（枠線やタイトルバー、移動を無効化）
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;

	// メインメニュー（GUIMainMenu）を表示するため、フルスクリーン化の計算を行う
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + 50.0f));
	ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - 50.0f));
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	// 背景を透明にする
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

	bool p_open = true;
	ImGui::Begin("MainDockSpaceWindow", &p_open, window_flags);

	ImGui::PopStyleVar(4); // StyleVar の復元
	ImGui::PopStyleColor(1);

	// 2. DockSpace 本体の生成
	ImGuiID dockspace_id = ImGui::GetID("MyEngineDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

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
			delete window;
			window = nullptr;
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

void GUIEditorManager::SaveCurrentLayout(const char* filePath)
{
	ImGui::SaveIniSettingsToDisk(filePath);
}

void GUIEditorManager::LoadCustomLayout(const char* filePath)
{
	ImGui::LoadIniSettingsFromDisk(filePath);
}

void GUIEditorManager::ApplyDefaultLayout_2by3()
{
	//TODO : 現在レイアウト設定が処理されないため今後作成予定
	/*
	ImGuiID dockspace_id = ImGui::GetID("MyEngineDockSpace");

	// 1. 既存のレイアウトを完全にクリア
	ImGui::DockBuilderRemoveNode(dockspace_id);
	ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_None);
 
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImVec2 customSize = viewport->WorkSize;
	customSize.y -= 50.0f; // タスクバーの高さ50px分、ドッキング領域を縦に縮める

	ImVec2 customPos = viewport->WorkPos;
	customPos.y += 50.0f;  // ドッキング領域の開始位置を50px下にずらす

	// サイズと位置を確定
	ImGui::DockBuilderSetNodeSize(dockspace_id, customSize);
	ImGui::DockBuilderSetNodePos(dockspace_id, customPos);

	// 3. 確定したルートサイズを基準に「分割（ハサミ入れ）」を開始する
	// 方向（Dir）と親ノードの指定を間違えないようにリレーします
	ImGuiID dock_id_hierarchy;
	ImGuiID dock_id_remaining = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dock_id_hierarchy);

	ImGuiID dock_id_inspector;
	ImGuiID dock_id_center = ImGui::DockBuilderSplitNode(dock_id_remaining, ImGuiDir_Right, 0.25f, nullptr, &dock_id_inspector);

	ImGuiID dock_id_scene;
	ImGuiID dock_id_game = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.5f, nullptr, &dock_id_scene);

	// 4. ウィンドウを割り当て
	ImGui::DockBuilderDockWindow("Hierarchy", dock_id_hierarchy);
	ImGui::DockBuilderDockWindow("Inspector", dock_id_inspector);
	ImGui::DockBuilderDockWindow("SceneView", dock_id_scene);
	ImGui::DockBuilderDockWindow("GameView", dock_id_game);

	// 4. レイアウト確定
	ImGui::DockBuilderFinish(dockspace_id);
	*/
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

void GUIEditorManager::AddSceneViewPanel(SceneViewPanel* panel)
{
	mSceneViewPanels.push_back(panel);
}

void GUIEditorManager::RemoveSceneViewPanel(SceneViewPanel* panel)
{
	auto it = std::find(mSceneViewPanels.begin(), mSceneViewPanels.end(), panel);
	if (it != mSceneViewPanels.end()) {
		mSceneViewPanels.erase(it);
	}
	else {
		Debug::Log("Not Found this camera.");
	}
}
