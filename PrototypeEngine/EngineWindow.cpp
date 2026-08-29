#include "EngineWindow.h"
#include "ComponentFactory.h"
#include "DebugManager.h"
#include "GUIEditorManager.h"
#include "GameViewPanel.h"
#include "PhysWorld.h"
#include "SceneEditorCamera.h"
#include "SceneManager.h"
#include "SceneViewPanel.h"
#include "ScriptHotReloadManager.h"
#include "WindowRenderProperty.h"
#include "FileOperationManager.h"
#include "AssetImporter.h"
#include "MaterialManager.h"

EngineState EngineWindow::mEngineState = EngineState::Run;

Renderer* EngineWindow::mRenderer = nullptr;

PhysWorld* EngineWindow::mPhysWorld = nullptr;

EngineWindow::EngineWindow() : mGameWindow(nullptr) {}

EngineWindow::~EngineWindow() {}

bool EngineWindow::EngineInitialize()
{
    // SDLの初期化
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult < 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Rendererの生成
    mRenderer = new Renderer();
    if (!mRenderer->Initialize(WindowRenderProperty::GetWidth(),
                               WindowRenderProperty::GetHeight()))
    {
        Debug::ErrorLog("Failed to initialize Renderer");
        delete mRenderer;
        mRenderer = nullptr;
        return false;
    }
    mPhysWorld = new PhysWorld();

    RegisterAllComponents();

    // SDL_ttfの初期化
    if (!TTF_Init())
    {
        Debug::ErrorLog("Failed to initialize SDL_ttf");
        return false;
    }
    // エンジン内部の処理フレームの初期化
    Time::InitializeDeltaTime();
    // エンジン状態を実行中に設定
    EngineWindow::mEngineState = EngineState::Run;
    // ゲームウィンドウの生成と初期化
    mGameWindow = new GameWinMain();
    mGameWindow->Initialize();
    //Assetsフォルダ内のファイルを確認
    AssetImporter::CheckAndImportAssets();
    // 起動時に最初のシーンを初期化
    SceneManager::InitializeScenes();
    //  ImGuiの初期化処理
    GUIEditorManager::InitializeImGui(mRenderer->GetWindow(),
                                      mRenderer->GetContext());
    // 仮で一回更新を行う
    mGameWindow->GameRunLoop();
    // スクリプトのホットリロードマネージャーを生成
    mHotReloadManager = std::make_unique<ScriptHotReloadManager>();
    // スクリプトDLLをロードする
    mHotReloadManager.get()->Initialize();
    return true;
}

void EngineWindow::EngineProcessInput()
{
    const InputState& state = InputSystem::GetState();
    InputSystem::Update();

    // 入力操作
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ImGui用のイベント処理
        ImGui_ImplSDL3_ProcessEvent(&event);
        InputSystem::ProcessEvent(event);
        switch (event.type)
        {
            // 実行が終了するとtrue
        case SDL_EVENT_QUIT:
            if (!GUIEditorManager::IsPlaying())
            {
                mEngineState = EngineState::End;
            }
            break;
        }

        if (event.type == SDL_EVENT_DROP_FILE)
        {
            FileOperationManager::AddDroppedFile(event.drop.data);
        }
    }
    //-------------------------------------------------------
    // ゲームエンジン内の入力処理
    //-------------------------------------------------------
    // ESCキーを押してゲーム入力を解除
    // auto window = GUIEditorManager::GetEditorWindow("GameView");
    if (state.Keyboard.GetKeyDown(
            KEY_ESCAPE)) //||(window != nullptr && !window->IsMouseHovered()))
    {
        if (InputContextManager::IsGameInputActive())
        {
            InputContextManager::SetContext(InputContext::Engine);
        }
    }

    // シーンビューのエディターカメラ入力
    GUIEditorManager::InputUpdateImGuiState();
    // シーンの入力処理
    if (GUIEditorManager::IsPlaying() && !GUIEditorManager::IsPaused())
    {
        if (InputContextManager::IsGameInputActive())
        {
            // ゲームが実行中なら
            mGameWindow->InputUpdate();
        }
    }

    InputSystem::PrepareForUpdate();
}

// エンジンプロジェクトのループ処理
void EngineWindow::EngineRunLoop()
{
    while (EngineWindow::mEngineState != EngineState::End)
    {
        // スクリプトDLLの変更を監視し、リロードが必要なら実行
        mHotReloadManager.get()->CheckForChanges(Time::gDeltaTime);
        // デルタタイム更新
        Time::UpdateDeltaTime();
        // 入力処理
        EngineProcessInput();
        // ImGuiの状態更新
        GUIEditorManager::UpdateImGuiState();
        // ここからゲーム内の更新開始
        // ゲームが開始したら
        if (GUIEditorManager::IsPlaying())
        {
            if (!GUIEditorManager::IsPaused())
            {
                // 開始した瞬間なら
                if (GUIEditorManager::IsStarting())
                {
                    // 編集での変更があればそれを記録する
                    string startupScenePath =
                        EditorSettingsManager::GetInstance()
                            .GetLastOpenedScene();
                    SceneSerializer::WriteEditingSceneData(
                        startupScenePath, SceneManager::GetCurrentRunScene());

                    GUIEditorManager::ResetPointer();
                    GameStateClass::SetGameState(GameState::GamePlay);
                    GUIEditorManager::SetIsStarting(false);
                    // 静的バッチの構築
                    mRenderer->BuildStaticBatch();
                }
                mGameWindow->GameRunLoop();
            }
        }
        else
        {
            if (mGameWindow->LoadGame_Engine())
            {
                // ゲームのロードが成功した場合の処理
                mGameWindow->GameRunLoop();
            }
        }
        SceneManager::GetCurrentRunScene()->EditorUpdate(
            GUIEditorManager::IsPlaying());
        // 終了ボタンが押されたら
        if (GUIEditorManager::IsPushEnd())
        {
            // 実行終了時アンロードしているがここは作業記録の物を読み込む
            SceneManager::GamePlayEndInitilaizeScene();
            // 仮で一回更新を行う
            mGameWindow->GameRunLoop();
            GUIEditorManager::SetIsPushEnd(false);
        }
        EngineRender();
    }
}

void EngineWindow::EngineRender()
{
    // Rendererの描画開始
    mRenderer->StartDraw();
    // ImGuiの描画
    GUIEditorManager::RenderImGui();
    // Rendererの描画終了
    mRenderer->EndDraw();
}

void EngineWindow::EngineShutdown()
{
    // エンジンのシャットダウン処理
    if (mHotReloadManager)
    {
        mHotReloadManager.get()->UnloadScripts();
        mHotReloadManager.reset();
    }
    // ゲームウィンドウのシャットダウンと解放
    if (mGameWindow)
    {
        CommandManager::Shutdown();
        mGameWindow->Shutdown();
        delete mGameWindow;
        mGameWindow = nullptr;
    }
    SceneManager::ReleaseAllScenes();
    TTF_Quit();
    if (mPhysWorld)
    {
        delete mPhysWorld;
        mPhysWorld = nullptr;
    }
    MaterialManager::AllMaterialClear();
    // Rendererの解放
    if (mRenderer)
    {
        mRenderer->UnloadData();
        mRenderer->Shutdown();
        delete mRenderer;
        mRenderer = nullptr;
    }
    GUIEditorManager::ShutdownImGui();
    SDL_Quit();
}
