#include "GameViewPanel.h"
#include "SceneViewEditor.h"
#include "GBuffer.h"

GameViewPanel::GameViewPanel(Renderer* renderer)
	:GUIPanel(renderer)
{
}

void GameViewPanel::Draw(float width, float height, ImTextureRef ref)
{
	// �E�C���h�E�ʒu�ƃT�C�Y���Œ�
	ImGui::SetNextWindowPos(ImVec2(0.0f, (float)height * 0.5f));
	ImGui::SetNextWindowSize(ImVec2((float)width * 0.5f, (float)height * 0.5f));
	ImGui::Begin(GetName(), nullptr, ImGuiWindowFlags_NoCollapse);
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

		//�X�V����
		ImVec2 winPos = ImGui::GetCursorScreenPos();
		ImVec2 winSize = ImGui::GetContentRegionAvail();

		// GameView �̃T�C�Y���ς������ FBO �����T�C�Y
		if (mRenderer->GetGameSceneViewEditor()->NeedsResize(Vector2((int)winSize.x, (int)winSize.y)))
		{

		}
		GUIWinMain::SetGameWinPos(Vector2(winPos.x, winPos.y));
		GUIWinMain::SetGameWinSize(Vector2(winSize.x, winSize.y));


		//�`�揈��
		// �E�B���h�E�T�C�Y�ɍ��킹�ĕ`��
		ImGui::Image(
			ref,
			winSize,
			ImVec2(0, 1),  // uv0 (�㉺���]�ɒ���)
			ImVec2(1, 0)   // uv1
		);
	}
	ImGui::End();
}
