#include "GameViewPanel.h"
#include "SceneViewEditor.h"
#include "GBuffer.h"

GameViewPanel::GameViewPanel(Renderer* renderer)
	:GUIPanel(renderer)
{
}

void GameViewPanel::Initialize(float width, float height, ImTextureRef ref)
{
	mWidthPos = 0.0f;
	mHeightPos = height * 0.5f;
	mWidthSize = width * 0.5f;
	mHeightSize = height * 0.5f;
}

void GameViewPanel::Draw(float width, float height, ImTextureRef ref)
{
	// �E�C���h�E�ʒu�ƃT�C�Y���Œ�
	if (isResetLayout)
	{
		ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos));
		ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize));
		isResetLayout = false;
	}
	else
	{
		ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
	}
	if(ImGui::Begin(GetName(), nullptr, ImGuiWindowFlags_NoCollapse))
	{
		//���͏���
		MouseHoveredDisble();
		// �}�E�X�����̃E�B���h�E�ɂ��邩�ǂ�������
		if (WindowHoveredConfirmation() && InputSystem::GetState().Mouse.GetButton(SDL_BUTTON_LEFT))
		{
			if(InputContextManager::IsEngineInputActive())
			{
				// GameView�p�l���Ƀ}�E�X������Ă���Ƃ��̏���
				InputContextManager::SetContext(InputContext::Game);
			}
		}
		GUIPanelMenu();

		//�X�V����
		ImVec2 winSize = GetAspectRatio();
		ImVec2 winPos = ImGui::GetCursorScreenPos();

		// GameView �̃T�C�Y���ς������ FBO �����T�C�Y
		if (mRenderer->GetGameSceneViewEditor()->NeedsResize(Vector2((int)winSize.x, (int)winSize.y)))
		{

		}
		GUIWinMain::SetGameWinPos(Vector2(winPos.x, winPos.y));
		GUIWinMain::SetGameWinSize(Vector2(winSize.x, winSize.y));


		//�`�揈��
		// �E�B���h�E�T�C�Y�ɍ��킹�ĕ`��
		ImGui::Image(
			(ImTextureID)(intptr_t)mRenderer->GetGameSceneViewEditor()->GetSceneColorTex(),
			winSize,
			ImVec2(0, 1),  // uv0 (�㉺���]�ɒ���)
			ImVec2(1, 0)   // uv1
		);
	}
	ImGui::End();
}
