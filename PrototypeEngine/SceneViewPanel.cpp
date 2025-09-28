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
	mWidthSize = (width * 0.5f) - mWidthPos;
	mHeightSize = (height * 0.5f) - mHeightPos;
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
	ImVec2 winsize = ImVec2(mWidthSize, mHeightSize);
	// �E�C���h�E�ʒu�ƃT�C�Y���Œ�
	if (isResetLayout)
	{
		ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos));
		ImGui::SetNextWindowSize(winsize);
		isResetLayout = false;
	}
	else
	{
		ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
		ImGui::SetNextWindowSize(winsize, ImGuiCond_Once);
	}
	if(ImGui::Begin(GetName(), nullptr, ImGuiWindowFlags_NoCollapse))
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
		GUIPanelMenu();

		ImVec2 size = GetAspectRatio();
		// SceneView �̃e�N�X�`����\��
		ImGui::Image(mRenderer->GetSceneViewEditor()->GetSceneColorTex(),
					 size,
					 ImVec2(0, 1),
					 ImVec2(1, 0));
	}
	ImGui::End();
}
