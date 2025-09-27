#include "GUIWinMain.h"
#include "Renderer.h"
#include "SceneViewEditor.h"
#include "ShadowMap.h"
#include "GBuffer.h"

#include "GUIMainMenu.h"
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

vector<GUIPanel*> GUIWinMain::mGUIPanel;

GUIMainMenu* GUIWinMain::mMainMenu = nullptr;

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

	int windowWidth = WindowRenderProperty::GetWidth();
	int windowHeight = WindowRenderProperty::GetHeight();

	mMainMenu = new GUIMainMenu(mRenderer);

	mToolbarPanel = new ToolbarPanel(mRenderer);

	mGameViewPanel = new GameViewPanel(mRenderer);

	mSceneViewPanel = new SceneViewPanel(mRenderer);

	mHierarchyPanel = new HierarchyPanel(mRenderer);

	mProjectPanel = new ProjectPanel(mRenderer);

	mSelectItemPanel = new SelectItemPanel(mRenderer);

	mGUIPanel.push_back(mMainMenu);
	mGUIPanel.push_back(mToolbarPanel);
	mGUIPanel.push_back(mGameViewPanel);
	mGUIPanel.push_back(mSceneViewPanel);
	mGUIPanel.push_back(mHierarchyPanel);
	mGUIPanel.push_back(mProjectPanel);
	mGUIPanel.push_back(mSelectItemPanel);

	for (int i = 0; i < mGUIPanel.size(); i++)
	{
		mGUIPanel[i]->Initialize(windowWidth, windowHeight);
	}

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

	for (int i = 0; i < mGUIPanel.size(); i++)
	{
		mGUIPanel[i]->Draw(windowWidth, windowHeight);
	}

	/*
	{
		mMainMenu->Draw(windowWidth, windowHeight);
	}
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
	*/



	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIWinMain::ShutdownImGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();


	if (mMainMenu)
	{
		delete mMainMenu;
		mMainMenu = nullptr;
	}
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

	mGUIPanel.clear();
}