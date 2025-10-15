#include "EngineWindow.h"
#include "BaseScene.h"
#include "SceneEditorCamera.h"
#include "GameViewPanel.h"
#include "ComponentFactory.h"

EngineState EngineWindow::mEngineState = EngineState::Run;

Renderer* EngineWindow::mRenderer = nullptr;

SceneEditorCamera* EngineWindow::mSceneEditorCamera = nullptr;

EngineWindow::EngineWindow()
{
}

EngineWindow::~EngineWindow()
{
	
}

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
	if (!mRenderer->Initialize(WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight()))
	{
		Debug::ErrorLog("Failed to initialize Renderer");
		delete mRenderer;
		mRenderer = nullptr;
		return false;
	}

	RegisterAllComponents();

	// SDL_ttfの初期化
	if (!TTF_Init())
	{
		Debug::ErrorLog("Failed to initialize SDL_ttf");
		return false;
	}

	Time::InitializeDeltaTime();

	EngineWindow::mEngineState = EngineState::Run;

	

	mGameWindow = new GameWinMain();
	mGameWindow->Initialize();
	
	//起動時に最初のシーンを初期化
	SceneManager::InitializeScenes();
	//  ImGuiの初期化処理
	GUIWinMain::InitializeImGui(mRenderer->GetWindow(), mRenderer->GetContext());
	//仮で一回更新を行う
	mGameWindow->GameRunLoop();

	mSceneEditorCamera = new SceneEditorCamera();
	GameStateClass::mDebugFrag = true;
	return true;
}

void EngineWindow::EngineProcessInput()
{
	const InputState& state = InputSystem::GetState();
	InputSystem::Update();

	//入力操作
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		// ImGui用のイベント処理
		ImGui_ImplSDL3_ProcessEvent(&event);
		InputSystem::ProcessEvent(event);
		switch (event.type)
		{
			//実行が終了するとtrue
		case SDL_EVENT_QUIT:
			if (!GUIWinMain::IsPlaying())
			{
				mEngineState = EngineState::End;
			}
			break;
		}
	}
	//-------------------------------------------------------
	// ゲームエンジン内の入力処理
	//-------------------------------------------------------
	//ESCキーを押してゲーム入力を解除
	if (state.Keyboard.GetKeyDown(KEY_ESCAPE)||!GUIWinMain::GetGameViewPanel()->IsMouseHovered())
	{
		if (InputContextManager::IsGameInputActive())
		{
			InputContextManager::SetContext(InputContext::Engine);
		}
	}

	// Debugビルドの場合の処理
	if (state.Keyboard.GetKeyDown(SDL_SCANCODE_F1))
	{
		GameStateClass::mDebugFrag = !GameStateClass::mDebugFrag;
	}
	//シーンビューのエディターカメラ入力
	mSceneEditorCamera->ProcessInput(state);
	//シーンの入力処理
	if (GUIWinMain::IsPlaying()&& !GUIWinMain::IsPaused())
	{
		if (InputContextManager::IsGameInputActive())
		{
			//ゲームが実行中なら
			mGameWindow->InputUpdate();
		}
	}

	InputSystem::PrepareForUpdate();
}

//エンジンプロジェクトのループ処理
void EngineWindow::EngineRunLoop()
{
	while (EngineWindow::mEngineState != EngineState::End)
	{
		//デルタタイム更新
		Time::UpdateDeltaTime();
		//入力処理
		EngineProcessInput();
		//ImGuiの状態更新
		GUIWinMain::UpdateImGuiState();
		//エディター用カメラの更新
		mSceneEditorCamera->Update();

		//ここからゲーム内の更新開始
		//ゲームが開始したら
		if (GUIWinMain::IsPlaying())
		{
			if (!GUIWinMain::IsPaused())
			{
				//開始した瞬間なら
				if (GUIWinMain::IsStarting())
				{
					GUIWinMain::ResetPointer();
					//Rendererのものもアンロード
					//mRenderer->UnloadData();
					//現在のシーンのオブジェクト、画像などをアンロード
					//SceneManager::GetNowScene()->UnloadData();
					GameStateClass::SetGameState(GameState::GamePlay);
					//新しいシーンの初期化
					//SceneManager::GetNowScene()->Initialize();
					//Rendererのシーンも変更
					//mRenderer->SetBaseScene(SceneManager::GetNowScene());

					GUIWinMain::SetIsStarting(false);
				}
				mGameWindow->GameRunLoop();
			}
		}
		else
		{
			mGameWindow->LoadGame_Engine();
			SceneManager::GetNowScene()->EditorUpdate();
		}
		//終了ボタンが押されたら
		if(GUIWinMain::IsPushEnd())
		{
			SceneManager::PlayEndInitilaizeScene();
		    //仮で一回更新を行う
			mGameWindow->GameRunLoop();
			GUIWinMain::SetIsPushEnd(false);
		}
		EngineRender();
	}
}

void EngineWindow::EngineRender()
{
	//Rendererの描画開始
	mRenderer->StartDraw();
	//ImGuiの描画
	GUIWinMain::RenderImGui();
	//Rendererの描画終了
	mRenderer->EndDraw();
}

void EngineWindow::EngineUnloadData()
{

	if(mGameWindow)
	{
		mGameWindow->Shutdown();
		delete mGameWindow;
		mGameWindow = nullptr;
	}
	SceneManager::ReleaseAllScenes();
}

void EngineWindow::EngineShutdown()
{
	if (mGameWindow)
	{
		mGameWindow->Shutdown();
		delete mGameWindow;
		mGameWindow = nullptr;
	}
	SceneManager::ReleaseAllScenes();
	TTF_Quit();
	if (mRenderer)
	{
		mRenderer->UnloadData();
		mRenderer->Shutdown();
		delete mRenderer;
		mRenderer = nullptr;
	}
	if (mSceneEditorCamera)
	{
		delete mSceneEditorCamera;
		mSceneEditorCamera = nullptr;
	}
	GUIWinMain::ShutdownImGui();
	SDL_Quit();
}
