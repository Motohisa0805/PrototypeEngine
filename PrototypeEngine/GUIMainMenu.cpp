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
	ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize));
	ImGui::Begin("MainMenu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	// メインメニューバーの開始
	if (ImGui::BeginMainMenuBar())
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
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Initialize"))
			{

			}
			if (ImGui::MenuItem("EditorScene"))
			{

			}
			if (ImGui::MenuItem("GameScene"))
			{

			}

			ImGui::EndMenu();
		}

		// メインメニューバーの終了
		ImGui::EndMainMenuBar();
	}
	ImGui::End();
}