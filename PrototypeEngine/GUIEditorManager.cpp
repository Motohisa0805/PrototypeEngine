#include "GUIEditorManager.h"
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

Vector2 GUIEditorManager::mGameWinPos = Vector2::Zero;
Vector2 GUIEditorManager::mGameWinSize = Vector2::Zero;

Vector2 GUIEditorManager::mSceneWinSize = Vector2::Zero;

std::unordered_map<string, EditorWindow*> GUIEditorManager::mEditorWindows;

bool GUIEditorManager::InitializeImGui(SDL_Window* window, SDL_GLContext glContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplSDL3_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 330");

	//GUI用のフォントを読み込む
	//フォントパス(Libraryフォルダーにアクセス)
	string fontpath = "Library/Noto_Sans_JP/static/NotoSansJP-Bold.ttf";
	//ImGuiのフォントクラスを参照
	ImGuiIO& io = ImGui::GetIO();
	//新しいフォントを読み込み
	ImFont* font = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 20.0f);
	if (font)
	{
		io.FontDefault = font;
	}

	int windowWidth = WindowRenderProperty::GetWidth();
	int windowHeight = WindowRenderProperty::GetHeight();

	RegisterAllEditorWindows(mRenderer);
	
	AddEditorWindow(EditorWindowFactory::CreateEditorWindow("GUIMainMenu"));
	AddEditorWindow(EditorWindowFactory::CreateEditorWindow("Toolbar"));
	AddEditorWindow(EditorWindowFactory::CreateEditorWindow("GameView"));
	AddEditorWindow(EditorWindowFactory::CreateEditorWindow("SceneView"));
	AddEditorWindow(EditorWindowFactory::CreateEditorWindow("Hierarchy"));
	AddEditorWindow(EditorWindowFactory::CreateEditorWindow("Project"));
	AddEditorWindow(EditorWindowFactory::CreateEditorWindow("Inspector"));

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
	for (auto window : mEditorWindows) {
		window.second->ClearPointer();
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

	for (auto window : mEditorWindows) {
		window.second->Draw(windowWidth, windowHeight);
	}

	// 描画が終わった後、閉じられたウィンドウを非表示に
	// イテレータの破損を防ぐため、削除対象を一時的にリストアップして後から消す
	std::vector<EditorWindow*> toDelete;
	for (auto pair : mEditorWindows) {
		if (!pair.second->IsShow()) { // パネルが閉じられていたら
			toDelete.push_back(pair.second);
		}
	}

	// リストアップしたウィンドウを安全に削除
	for (auto window : toDelete) {
		RemoveEditorWindow(window);
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

	EditorWindowFactory::UnregisterAllEditorWindows();
	mEditorWindows.clear();
	
	if (SelectionManager::GetSelectedActor())
	{
		SelectionManager::SetSelectedActor(nullptr);
	}

	EditorTextureManager::GetInstance().AllRelease();
}

void GUIEditorManager::AddEditorWindow(EditorWindow* window)
{
	auto it = mEditorWindows.find(window->GetID());
	if (it == mEditorWindows.end()) {
		mEditorWindows.emplace(window->GetID(), window);
		window->Initialize(WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight());
		window->Enabled();
	}
	else {
		Debug::Log("This Window already created");
	}
}

void GUIEditorManager::RemoveEditorWindow(EditorWindow* window)
{
	auto it = mEditorWindows.find(window->GetID());
	if (it != mEditorWindows.end()) {
		mEditorWindows.erase(it->first);
	}
	else {
		Debug::Log("Not founded this window");
	}
}

void GUIEditorManager::DeleteEditorWindow(EditorWindow* window)
{
	if (!window) return;

	auto it = mEditorWindows.find(window->GetID());
	if (it != mEditorWindows.end()) {
		delete it->second; // 1. 先にインスタンスを解放
		mEditorWindows.erase(it); // 2. その後マップからイテレータを使って削除
	}
	else {
		Debug::Log("Not found this window");
	}
}

EditorWindow* GUIEditorManager::GetEditorWindow(string key)
{
	auto it = mEditorWindows.find(key);
	if (it != mEditorWindows.end()) {
		return it->second;
	}
	return nullptr;
}
