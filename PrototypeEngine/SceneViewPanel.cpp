#include "SceneViewPanel.h"
#include "GBuffer.h"
#include "SceneEditorCamera.h"
#include "SceneViewEditor.h"

SceneViewPanel::SceneViewPanel(Renderer* renderer)
    : EditorWindow(renderer)
    , mSceneEditorCamera(nullptr)
    , mSceneViewEditor(nullptr)
    , mSceneBuffer(nullptr)
    , mSceneWinSize(Vector2())
    , mIsShadowFrag(false)
    , mIsDebugGridFrag(true)
{
    mID = "SceneView";
    // エディター用カメラの生成
    mSceneEditorCamera = new SceneEditorCamera(nullptr);
    mSceneEditorCamera->SetSceneViewPanel(this);

    GUIEditorManager::AddSceneViewPanel(this);
}

SceneViewPanel::~SceneViewPanel()
{
    GUIEditorManager::RemoveSceneViewPanel(this);
    if (mSceneEditorCamera)
    {
        delete mSceneEditorCamera;
        mSceneEditorCamera = nullptr;
    }

    if (mSceneBuffer)
    {
        mSceneBuffer->Destroy();
        delete mSceneBuffer;
        mSceneBuffer = nullptr;
    }

    if (mSceneViewEditor)
    {
        delete mSceneViewEditor;
        mSceneViewEditor = nullptr;
    }
}

void SceneViewPanel::Initialize(float width, float height, ImTextureRef ref)
{
    mWidthPos   = 0.0f;
    mHeightPos  = 55.0f;
    mWidthSize  = (width * 0.5f) - mWidthPos;
    mHeightSize = (height * 0.5f) - mHeightPos;
    EditorWindow::Initialize(width, height, ref);
    ResetLayoutFunction();

    mSceneBuffer = new GBuffer();
    if (!mSceneBuffer->Create(width, height))
    {
        SDL_Log("Failed to create mSceneBuffer.");
    }

    // エディターとゲームのシーンビューのFBOを作成
    // エディターシーンのFBOを作成
    mSceneViewEditor = new SceneViewEditor();
    mSceneViewEditor->CreateSceneFBO(width, height);
}

bool SceneViewPanel::MouseHoveredDisble()
{
    isMouseHovered = false;
    return true;
}

void SceneViewPanel::Draw(float width, float height)
{
    EditorWindow::Draw(width, height);
    if (ImGui::Begin(GetImGuiWindowID().c_str(), &mIsShow,
                     ImGuiWindowFlags_NoCollapse))
    {
        // デバッグモード切り替えボタン
        ImGuiHelper::FragTextButton("Grid:", ImVec2(0.0f, 0.0f),
                                    mIsDebugGridFrag);
        // 同じ行に固定
        ImGui::SameLine();
        // シャドウマップの表示切り替えボタン
        ImGuiHelper::FragTextButton("Shadow:", ImVec2(0.0f, 0.0f),
                                    mIsShadowFrag);

        ImVec2 winsize = ImVec2(mWidthSize, mHeightSize);
        // SceneView のサイズが変わったら FBO をリサイズ
        if (mSceneViewEditor->NeedsResize(
                Vector2((int)winsize.x, (int)winsize.y)))
        {
            mSceneViewEditor->CreateSceneFBO((int)winsize.x, (int)winsize.y);
            mSceneBuffer->Resize((int)winsize.x, (int)winsize.y);
            mSceneWinSize = Vector2(winsize.x, winsize.y);
        }

        MouseHoveredDisble();
        // マウスがこのウィンドウにあるかどうか判定
        WindowHoveredConfirmation();

        ImVec2 size = GetAspectRatio();
        // SceneView のテクスチャを貼る
        ImGui::Image(mSceneViewEditor->GetSceneColorTex(), size, ImVec2(0, 1),
                     ImVec2(1, 0));
    }
    ImGui::End();
}

void SceneViewPanel::InputUpdate()
{
    if (!mSceneEditorCamera)
        return;

    const InputState& state = InputSystem::GetState();

    mSceneEditorCamera->ProcessInput(state);
}

void SceneViewPanel::Update()
{
    if (!mSceneEditorCamera)
        return;

    mSceneEditorCamera->Update();
}
