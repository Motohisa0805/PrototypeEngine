#include "GUIWinMain.h"
#include "Renderer.h"
#include "SceneViewEditor.h"
#include "ShadowMap.h"
#include "GBuffer.h"

#include "ToolbarPanel.h"
#include "GameViewPanel.h"
#include "SceneViewPanel.h"
#include "HierarchyPanel.h"
#include "ProjectPanel.h"
#include "SelectItemPanel.h"

bool GUIWinMain::isPaused = false;

bool GUIWinMain::isPlaying = false;

bool GUIWinMain::isStarting = false;

bool GUIWinMain::isPushEnd = false;

bool GUIWinMain::isFrameByFrame = false;

Renderer* GUIWinMain::mRenderer = nullptr;

Vector2 GUIWinMain::mGameWinPos = Vector2::Zero;
Vector2 GUIWinMain::mGameWinSize = Vector2::Zero;

Vector2 GUIWinMain::mSceneWinSize = Vector2::Zero;

ToolbarPanel* GUIWinMain::mToolbarPanel = nullptr;

GameViewPanel* GUIWinMain::mGameViewPanel = nullptr;

SceneViewPanel* GUIWinMain::mSceneViewPanel = nullptr;

HierarchyPanel* GUIWinMain::mHierarchyPanel = nullptr;

ProjectPanel* GUIWinMain::mProjectPanel = nullptr;

SelectItemPanel* GUIWinMain::mSelectItemPanel = nullptr;

bool GUIWinMain::InitializeImGui(SDL_Window* window, SDL_GLContext glContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplSDL3_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 330");

	mToolbarPanel = new ToolbarPanel(mRenderer);
	mToolbarPanel->Initialize();

	mGameViewPanel = new GameViewPanel(mRenderer);

	mSceneViewPanel = new SceneViewPanel(mRenderer);

	mHierarchyPanel = new HierarchyPanel(mRenderer);

	mProjectPanel = new ProjectPanel(mRenderer);

	mSelectItemPanel = new SelectItemPanel(mRenderer);

	return true;
}

void GUIWinMain::UpdateImGuiState()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	//  デモウィンドウの描画
	ImGui::ShowDemoWindow();
}

void GUIWinMain::RenderImGui()
{
	// ここでImGuiの描画を行う
	// 画面サイズ（SDLで取得したウィンドウ幅/高さ）
	int windowWidth = WindowRenderProperty::GetWidth();
	int windowHeight = WindowRenderProperty::GetHeight();
	//再生/一時停止/停止ボタンの状態を管理
	{
		mToolbarPanel->Draw(windowWidth, windowHeight);
	}
	//X = 0の位置にウインドウを配置
	//Xサイズ windowWidth * 0.5f
	{
		mSceneViewPanel->Draw((float)windowWidth, (float)windowHeight, (ImTextureID)(intptr_t)mRenderer->GetSceneViewEditor()->GetSceneColorTex());
	}
	{
		mGameViewPanel->Draw((float)windowWidth, (float)windowHeight, (ImTextureID)(intptr_t)mRenderer->GetGameSceneViewEditor()->GetSceneColorTex());
	}
	//X = windowWidth * 0.5fの位置にウインドウを配置
	//Xサイズ windowWidth * 0.15f
	//Hierarchyウィンドウの描画
	{
		mHierarchyPanel->Draw(windowWidth,windowHeight);
	}
	//X = windowWidth * 0.65fの位置にウインドウを配置
	//Xサイズ windowWidth * 0.15f
	//Projectウィンドウの描画
	{
		mProjectPanel->Draw(windowWidth, windowHeight);
	}
	//X = windowWidth * 0.8fの位置にウインドウを配置
	//Xサイズ windowWidth * 0.2f
	//SelectItemウィンドウの描画
	{
		mSelectItemPanel->Draw(windowWidth, windowHeight);
	}



	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIWinMain::ShutdownImGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	if (mToolbarPanel)
	{
		delete mToolbarPanel;
		mToolbarPanel = nullptr;
	}
	if (mGameViewPanel)
	{
		delete mGameViewPanel;
		mGameViewPanel = nullptr;
	}
	if (mSceneViewPanel)
	{
		delete mSceneViewPanel;
		mSceneViewPanel = nullptr;
	}
	if (mHierarchyPanel)
	{
		delete mHierarchyPanel;
		mHierarchyPanel = nullptr;
	}
	if (mProjectPanel)
	{
		delete mProjectPanel;
		mProjectPanel = nullptr;
	}
	if (mSelectItemPanel)
	{
		delete mSelectItemPanel;
		mSelectItemPanel = nullptr;
	}
}