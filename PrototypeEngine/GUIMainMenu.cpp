#include "GUIMainMenu.h"
#include "EditorSettingsManager.h"
#include "SceneSerializer.h"
#include "SceneManager.h"
#include "DebugManager.h"
#include "GUIWinMain.h"
#include "EngineWindow.h"
#include "BaseScene.h"

GUIMainMenu::GUIMainMenu(Renderer* renderer)
	:GUIPanel(renderer)
{
}

GUIMainMenu::~GUIMainMenu()
{
}

void GUIMainMenu::Initialize(float width, float height, ImTextureRef ref)
{
	mWidthPos = 0.0f;
	mHeightPos = 0.0f;
	mWidthSize = width;
	mHeightSize = 25.0f;
	GUIPanel::Initialize(width, height, ref);
}

void GUIMainMenu::Draw(float width, float height, ImTextureRef ref)
{
	SetPopupColorTheme();
	ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
	ImGui::Begin("MainMenu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
									  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	// メインメニューバーの開始
	if (ImGui::BeginMainMenuBar())
	{
		// "File" メニューの開始
		FileMenuDraw();
		// "Editor" メニューの開始
		EditorMenuDraw();
		// "Assets" メニューの開始
		AssetMenuDraw();
		// "GameObject" メニューの開始
		GameObjectMenuDraw();
		//"Component"メニュの開始
		ComponentMenuDraw();
		// "Window" メニューの開始
		WindowMenuDraw();

		// メインメニューバーの終了
		ImGui::EndMainMenuBar();
	}
	ImGui::End();
	ResetPopupColorTheme();
}

void GUIMainMenu::FileMenuDraw()
{
	// "File" メニューの開始
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Save","Ctrl+S"))
		{
			string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
			SceneSerializer::SaveScene(startupScenePath, SceneManager::GetNowScene());
			EditorSettingsManager::SetSaveFlag(false);
		}

		if (ImGui::MenuItem("Build Game(Incomplete)"))
		{
			// Build Gameがクリックされた時の処理
			Debug::Log("Build Game clicked!");
		}

		if (ImGui::MenuItem("Exit"))
		{
			EngineWindow::SetEngineState(EngineState::End);
		}

		// "File" メニューの終了
		ImGui::EndMenu();
	}
}

void GUIMainMenu::EditorMenuDraw()
{
	if (ImGui::BeginMenu("Editor"))
	{
		if (ImGui::MenuItem("Incomplete"))
		{

		}
		ImGui::EndMenu();
	}
}

void GUIMainMenu::AssetMenuDraw()
{
	if (ImGui::BeginMenu("Assets"))
	{
		if (ImGui::MenuItem("Incomplete"))
		{

		}
		ImGui::EndMenu();
	}
}

void GUIMainMenu::GameObjectMenuDraw()
{
	if (ImGui::BeginMenu("GameObject"))
	{
		if (ImGui::MenuItem("Incomplete"))
		{

		}
		ImGui::EndMenu();
	}
}

void GUIMainMenu::ComponentMenuDraw()
{
	if (ImGui::BeginMenu("Component"))
	{
		if (ImGui::MenuItem("Incomplete"))
		{

		}
		ImGui::EndMenu();
	}
}

void GUIMainMenu::WindowMenuDraw()
{
	if (ImGui::BeginMenu("Window"))
	{
		if (ImGui::MenuItem("All GUI Initialization of position"))
		{
			// 全てのGUIのレイアウトを初期位置に戻す処理
			for (int i = 0; i < GUIWinMain::GetGUIPanels().size(); ++i)
			{
				GUIWinMain::GetGUIPanels()[i]->EnableResetLayout();
			}
		}
		ImGui::EndMenu();
	}
}

void GUIMainMenu::SetPopupColorTheme()
{
	GUIPanel::SetPopupColorTheme();
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.9f, 0.9f, 0.9f, 0.9f));
}

void GUIMainMenu::ResetPopupColorTheme()
{
	ImGui::PopStyleColor(4);
}
