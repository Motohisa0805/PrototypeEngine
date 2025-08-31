#include "GameViewPanel.h"
#include "SceneViewEditor.h"
#include "GBuffer.h"

GameViewPanel::GameViewPanel(Renderer* renderer)
	:mRenderer(renderer)
{
}

void GameViewPanel::Draw(float width, float height, ImTextureRef ref)
{
	// �E�C���h�E�ʒu�ƃT�C�Y���Œ�
	ImGui::SetNextWindowPos(ImVec2(0.0f, (float)height * 0.5f));
	ImGui::SetNextWindowSize(ImVec2((float)width * 0.5f, (float)height * 0.5f));
	ImGui::Begin(GetName(), nullptr, ImGuiWindowFlags_NoCollapse);
	{
		ImVec2 winPos = ImGui::GetCursorScreenPos();
		ImVec2 winSize = ImGui::GetContentRegionAvail();

		// GameView �̃T�C�Y���ς������ FBO �����T�C�Y
		if (mRenderer->GetSceneViewEditor()->NeedsResize(Vector2((int)winSize.x, (int)winSize.y)))
		{

		}
		GUIWinMain::SetGameWinPos(Vector2(winPos.x, winPos.y));
		GUIWinMain::SetGameWinSize(Vector2(winSize.x, winSize.y));

		// �E�B���h�E�T�C�Y�ɍ��킹�ĕ`��
		ImVec2 size = ImGui::GetContentRegionAvail();
		ImGui::Image(
			ref,
			size,
			ImVec2(0, 1),  // uv0 (�㉺���]�ɒ���)
			ImVec2(1, 0)   // uv1
		);
	}
	ImGui::End();
}
