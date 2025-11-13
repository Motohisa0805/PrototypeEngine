#include "SceneViewPanel.h"
#include "SceneViewEditor.h"
#include "GBuffer.h"
#include "EngineWindow.h"

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
	GUIPanel::Initialize(width, height, ref);
}

bool SceneViewPanel::MouseHoveredDisble()
{
	isMouseHovered = false;
	return true;
}

void SceneViewPanel::Draw(float width, float height, ImTextureRef ref)
{
	ResetLayoutFunction();
	ImVec2 winsize = ImVec2(mWidthSize, mHeightSize);
	if(ImGui::Begin(GetName(), nullptr, ImGuiWindowFlags_NoCollapse))
	{
		//デバッグモード切り替えボタン
		if (ImGui::Button("G", ImVec2(0.0f, 0.0f)))
		{
			GameStateClass::gDebugGridFrag = !GameStateClass::gDebugGridFrag;
		}

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

		ImVec2 size = GetAspectRatio();
		// SceneView のテクスチャを貼る
		ImGui::Image(mRenderer->GetSceneViewEditor()->GetSceneColorTex(),
					 size,
					 ImVec2(0, 1),
					 ImVec2(1, 0));
	}
	ImGui::End();
}
