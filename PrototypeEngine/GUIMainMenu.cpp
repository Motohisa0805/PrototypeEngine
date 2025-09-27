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
	// ���C�����j���[�o�[�̊J�n
	if (ImGui::BeginMainMenuBar())
	{
		// "File" ���j���[�̊J�n
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Build Game"))
			{
				// Build Game���N���b�N���ꂽ���̏���
				Debug::Log("Build Game clicked!");
			}

			// "File" ���j���[�̏I��
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

		// ���C�����j���[�o�[�̏I��
		ImGui::EndMainMenuBar();
	}
	ImGui::End();
}