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
	// ���C�����j���[�o�[�̊J�n
	if (ImGui::BeginMainMenuBar())
	{
		// "File" ���j���[�̊J�n
		FileMenuDraw();
		ViewMenuDraw();

		// ���C�����j���[�o�[�̏I��
		ImGui::EndMainMenuBar();
	}
	ImGui::End();
}

void GUIMainMenu::FileMenuDraw()
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
}

void GUIMainMenu::ViewMenuDraw()
{
	if (ImGui::BeginMenu("View"))
	{
		if (ImGui::MenuItem("All GUI Initialization of position"))
		{
			// �S�Ă�GUI�̃��C�A�E�g�������ʒu�ɖ߂�����
			for (int i = 0; i < GUIWinMain::GetGUIPanels().size(); ++i)
			{
				GUIWinMain::GetGUIPanels()[i]->EnableResetLayout();
			}
		}
		ImGui::EndMenu();
	}
}
