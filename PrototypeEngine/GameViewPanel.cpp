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
	// ウインドウ位置とサイズを固定
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
		//入力処理
		MouseHoveredDisble();
		// マウスがこのウィンドウにあるかどうか判定
		if (WindowHoveredConfirmation() && InputSystem::GetState().Mouse.GetButton(SDL_BUTTON_LEFT))
		{
			if(InputContextManager::IsEngineInputActive())
			{
				// GameViewパネルにマウスが乗っているときの処理
				InputContextManager::SetContext(InputContext::Game);
			}
		}
		GUIPanelMenu();

		//更新処理
		ImVec2 winSize = GetAspectRatio();
		ImVec2 winPos = ImGui::GetCursorScreenPos();

		// GameView のサイズが変わったら FBO をリサイズ
		if (mRenderer->GetGameSceneViewEditor()->NeedsResize(Vector2((int)winSize.x, (int)winSize.y)))
		{

		}
		GUIWinMain::SetGameWinPos(Vector2(winPos.x, winPos.y));
		GUIWinMain::SetGameWinSize(Vector2(winSize.x, winSize.y));


		//描画処理
		// ウィンドウサイズに合わせて描画
		ImGui::Image(
			(ImTextureID)(intptr_t)mRenderer->GetGameSceneViewEditor()->GetSceneColorTex(),
			winSize,
			ImVec2(0, 1),  // uv0 (上下反転に注意)
			ImVec2(1, 0)   // uv1
		);
	}
	ImGui::End();
}
