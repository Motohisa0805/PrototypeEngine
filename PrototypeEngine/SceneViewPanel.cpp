#include "SceneViewPanel.h"
#include "SceneViewEditor.h"
#include "GBuffer.h"

SceneEditorCamera* SceneViewPanel::mSceneEditorCamera = nullptr;

SceneViewPanel::SceneViewPanel(Renderer* renderer)
	:EditorWindow(renderer)
{
	mID = "SceneView";
	//エディター用カメラの生成
	mSceneEditorCamera = new SceneEditorCamera(nullptr);
	mSceneEditorCamera->SetSceneViewPanel(this);
}

SceneViewPanel::~SceneViewPanel()
{
	if (mSceneEditorCamera)
	{
		delete mSceneEditorCamera;
		mSceneEditorCamera = nullptr;
	}
}

void SceneViewPanel::Initialize(float width, float height, ImTextureRef ref)
{
	mWidthPos = 0.0f;
	mHeightPos = 55.0f;
	mWidthSize = (width * 0.5f) - mWidthPos;
	mHeightSize = (height * 0.5f) - mHeightPos;
	EditorWindow::Initialize(width, height, ref);
	ResetLayoutFunction();
}

bool SceneViewPanel::MouseHoveredDisble()
{
	isMouseHovered = false;
	return true;
}

void SceneViewPanel::Draw(float width, float height)
{
	EditorWindow::Draw(width, height);
	if(ImGui::Begin(GetID().c_str(), &mIsShow, ImGuiWindowFlags_NoCollapse))
	{
		//デバッグモード切り替えボタン
		ImGuiHelper::FragTextButton("Grid:", ImVec2(0.0f, 0.0f), GameStateClass::gDebugGridFrag);
		//同じ行に固定
		ImGui::SameLine();
		//シャドウマップの表示切り替えボタン
		ImGuiHelper::FragTextButton("Shadow:", ImVec2(0.0f, 0.0f), GameStateClass::gShadowFrag);

		ImVec2 winsize = ImVec2(mWidthSize, mHeightSize);
		// SceneView のサイズが変わったら FBO をリサイズ
		if (mRenderer->GetSceneViewEditor()->NeedsResize(Vector2((int)winsize.x, (int)winsize.y)))
		{
			mRenderer->GetSceneViewEditor()->CreateSceneFBO((int)winsize.x, (int)winsize.y);
			mRenderer->GetSceneBuffer()->Resize((int)winsize.x, (int)winsize.y);
			GUIEditorManager::SetSceneWinSize(Vector2(winsize.x, winsize.y));
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

void SceneViewPanel::InputCameraUpdate()
{
	if (!mSceneEditorCamera)return;

	const InputState& state = InputSystem::GetState();

	mSceneEditorCamera->ProcessInput(state);
}

void SceneViewPanel::CameraUpdate()
{
	if (!mSceneEditorCamera)return;

	mSceneEditorCamera->Update();
}
