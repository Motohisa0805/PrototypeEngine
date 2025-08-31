#include "GameViewPanel.h"
#include "SceneViewEditor.h"
#include "GBuffer.h"

GameViewPanel::GameViewPanel(Renderer* renderer)
	:mRenderer(renderer)
{
}

void GameViewPanel::Draw(float width, float height, ImTextureRef ref)
{
	// ウインドウ位置とサイズを固定
	ImGui::SetNextWindowPos(ImVec2(0.0f, (float)height * 0.5f));
	ImGui::SetNextWindowSize(ImVec2((float)width * 0.5f, (float)height * 0.5f));
	ImGui::Begin(GetName(), nullptr, ImGuiWindowFlags_NoCollapse);
	{
		ImVec2 winPos = ImGui::GetCursorScreenPos();
		ImVec2 winSize = ImGui::GetContentRegionAvail();

		// GameView のサイズが変わったら FBO をリサイズ
		if (mRenderer->GetSceneViewEditor()->NeedsResize(Vector2((int)winSize.x, (int)winSize.y)))
		{

		}
		GUIWinMain::SetGameWinPos(Vector2(winPos.x, winPos.y));
		GUIWinMain::SetGameWinSize(Vector2(winSize.x, winSize.y));

		// ウィンドウサイズに合わせて描画
		ImVec2 size = ImGui::GetContentRegionAvail();
		ImGui::Image(
			ref,
			size,
			ImVec2(0, 1),  // uv0 (上下反転に注意)
			ImVec2(1, 0)   // uv1
		);
	}
	ImGui::End();
}
