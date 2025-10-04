#include "GUIMainMenu.h"

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
}

void GUIMainMenu::Draw(float width, float height, ImTextureRef ref)
{
	ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
	ImGui::Begin("MainMenu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
									  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	// メインメニューバーの開始
	if (ImGui::BeginMainMenuBar())
	{
		// "File" メニューの開始
		FileMenuDraw();
		ViewMenuDraw();

		// メインメニューバーの終了
		ImGui::EndMainMenuBar();
	}
	ImGui::End();
}

void GUIMainMenu::FileMenuDraw()
{
	// "File" メニューの開始
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Build Game"))
		{
			// Build Gameがクリックされた時の処理
			Debug::Log("Build Game clicked!");
		}

		// "File" メニューの終了
		ImGui::EndMenu();
	}
}

void GUIMainMenu::ViewMenuDraw()
{
	if (ImGui::BeginMenu("View"))
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
