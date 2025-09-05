#include "SceneViewPanel.h"
#include "SceneViewEditor.h"
#include "GBuffer.h"

SceneViewPanel::SceneViewPanel(Renderer* renderer)
	:mRenderer(renderer)
{
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
	// ウインドウ位置とサイズを固定
	ImGui::SetNextWindowPos(ImVec2(0.0f, 30));
	ImVec2 winsize = ImVec2((float)width * 0.5f, (float)height * 0.5f);
	ImGui::SetNextWindowSize(winsize);
	ImGui::Begin(GetName(), nullptr);
	{
		// SceneView のサイズが変わったら FBO をリサイズ
		if (mRenderer->GetSceneViewEditor()->NeedsResize(Vector2((int)winsize.x, (int)winsize.y)))
		{
			mRenderer->GetSceneViewEditor()->CreateSceneFBO((int)winsize.x, (int)winsize.y);
			mRenderer->GetSceneBuffer()->Resize((int)winsize.x, (int)winsize.y);
			GUIWinMain::SetSceneWinSize(Vector2(winsize.x, winsize.y));
		}

		MouseHoveredDisble();
		//マウスがこのウィンドウにあるかどうか判定
		WindowHoveredConfirmation();

		ImVec2 size = ImGui::GetContentRegionAvail();
		// SceneView のテクスチャを貼る
		ImGui::Image(ref,
					 size,
					 ImVec2(0, 1),
					 ImVec2(1, 0));
	}
	ImGui::End();
}
