#include "SceneViewPanel.h"
#include "SceneViewEditor.h"
#include "GBuffer.h"

SceneViewPanel::SceneViewPanel(Renderer* renderer)
	:GUIPanel(renderer)
{
}

void SceneViewPanel::Initialize(float width, float height, ImTextureRef ref)
{
	mWidthPos = 0.0f;
	mHeightPos = 55.0f;
	mWidthSize = width * 0.5f;
	mHeightSize = height * 0.5f;
}

bool SceneViewPanel::MouseHoveredDisble()
{
	if (!InputSystem::GetState().Mouse.GetButton(SDL_BUTTON_RIGHT))
	{
		isMouseHovered = false;
	}
	return true;
}

void SceneViewPanel::Draw(float width, float height, ImTextureRef ref)
{
	// �E�C���h�E�ʒu�ƃT�C�Y���Œ�
	ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
	ImVec2 winsize = ImVec2(mWidthSize, mHeightSize);
	ImGui::SetNextWindowSize(winsize);
	if(ImGui::Begin(GetName(), nullptr))
	{
		// SceneView �̃T�C�Y���ς������ FBO �����T�C�Y
		if (mRenderer->GetSceneViewEditor()->NeedsResize(Vector2((int)winsize.x, (int)winsize.y)))
		{
			mRenderer->GetSceneViewEditor()->CreateSceneFBO((int)winsize.x, (int)winsize.y);
			mRenderer->GetSceneBuffer()->Resize((int)winsize.x, (int)winsize.y);
			GUIWinMain::SetSceneWinSize(Vector2(winsize.x, winsize.y));
		}

		MouseHoveredDisble();
		//�}�E�X�����̃E�B���h�E�ɂ��邩�ǂ�������
		WindowHoveredConfirmation();

		ImVec2 size = ImGui::GetContentRegionAvail();
		// SceneView �̃e�N�X�`����\��
		ImGui::Image(mRenderer->GetSceneViewEditor()->GetSceneColorTex(),
					 size,
					 ImVec2(0, 1),
					 ImVec2(1, 0));
	}
	ImGui::End();
}
