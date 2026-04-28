#include "GameViewPanel.h"
#include "SceneViewEditor.h"
#include "GBuffer.h"
#include "InputSystem.h"
#include "EngineWindow.h"

#include "SceneManager.h"
#include "BaseScene.h"

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
	GUIPanel::Initialize(width, height, ref);
}

void GameViewPanel::Draw(float width, float height, ImTextureRef ref)
{
	ResetLayoutFunction();
	ImGuiBackendFlags flag = ImGuiWindowFlags_NoCollapse;
	if (InputContextManager::IsGameInputActive())
	{
		flag |= ImGuiWindowFlags_NoMove;
	}
	if(ImGui::Begin(GetName(), nullptr, flag))
	{
		//デバッグモード切り替えボタン
		ImGuiHelper::FragTextButton("State:", ImVec2(0.0f, 0.0f), GameStateClass::gDebugStatesFrag);

		//入力処理
		MouseHoveredDisble();
		// マウスがこのウィンドウにあるかどうか判定
		if (WindowHoveredConfirmation() && InputSystem::GetState().Mouse.GetButton(SDL_BUTTON_LEFT))
		{
			if(InputContextManager::IsEngineInputActive())
			{
				// GameViewパネルにマウスが乗っているときの処理
				//InputContextManager::SetContext(InputContext::Game);
			}
		}

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
		//"State"ボタンでトグルされるフラグがtrueの場合のみ描画
		if (GameStateClass::gDebugStatesFrag)
		{
			const float padding = 10.0f;
			//1.オーバーレイウィンドウの表示位置を計算
			ImVec2 overlayPos = ImVec2(
				winPos.x + winSize.x - padding,
				winPos.y + padding
			);

			//2.描画するウインドウの位置と「アンカー」を設定
			ImGui::SetNextWindowPos(overlayPos, ImGuiCond_Appearing, ImVec2(1.0f, 0.0f));

			//3.ウインドウのスタイルを設定
			ImGui::SetNextWindowBgAlpha(0.35f);// 半透明にする

			//4.ウインドウのフラグを設定
			ImGuiWindowFlags overlayFlags =
				ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoMove;
			//5.オーバーレイウインドウの描画開始
			if (ImGui::Begin("StatesOverlay", nullptr, overlayFlags))
			{
				//タイトル
				ImGui::Text("Game Stats");
				ImGui::Separator();
				float time = Time::GetFrameRate();
				ImGui::Text("FPS: %.1f", time);
				int drawCalls = mRenderer->GetDrawCalls();
				ImGui::Text("Draw Calls: %d", drawCalls);
				int vertices = SceneManager::GetNowScene()->GetSceneAllVertices();
				ImGui::Text("Vertices: %d", vertices);
				ImGui::Text("Tris: %d", vertices / 3);
			}
			ImGui::End();
		}
	}
	ImGui::End();
}
