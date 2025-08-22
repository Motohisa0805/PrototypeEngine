#include "EngineWindow.h"
#include "BaseScene.h"

EngineState EngineWindow::mEngineState = EngineState::Run;

Renderer* EngineWindow::mRenderer = nullptr;

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

	if (GUIWinMain::IsPlaying())
	{
		//ゲームが実行中なら
		mGameWindow->InputUpdate();
	}

	InputSystem::PrepareForUpdate();
}

//エンジンプロジェクトのループ処理
void EngineWindow::EngineRunLoop()
{
	while (EngineWindow::mEngineState != EngineState::End)
	{
		Time::UpdateDeltaTime();

		EngineProcessInput();

		GUIWinMain::UpdateImGuiState();


		//ここからゲーム内の更新開始
		//ゲームが開始したら
		if (GUIWinMain::IsPlaying())
		{
			//開始した瞬間なら
			if (GUIWinMain::IsStarting())
			{
				//Rendererのものもアンロード
				mRenderer->UnloadData();
				//現在のシーンのオブジェクト、画像などをアンロード
				SceneManager::GetNowScene()->UnloadData();
				GameStateClass::SetGameState(GameState::GamePlay);
				//新しいシーンの初期化
				SceneManager::GetNowScene()->Initialize();
				//Rendererのシーンも変更
				mRenderer->SetBaseScene(SceneManager::GetNowScene());

				GUIWinMain::SetIsStarting(false);
			}
			mGameWindow->GameRunLoop();
		}


		//終了ボタンが押されたら
		if( GUIWinMain::IsPushEnd() )
		{
			//現在のシーンのオブジェクト、画像などをアンロード
			SceneManager::GetNowScene()->UnloadData();
			//Rendererのものもアンロード
			mRenderer->UnloadData();
			GameStateClass::SetGameState(GameState::GameEnd);
			SceneManager::ReleaseAllScenes();
			SceneManager::InitializeScenes();
		    //仮で一回更新を行う
			mGameWindow->GameRunLoop();

			GUIWinMain::SetIsPushEnd(false);
		}
		EngineRender();

	}
}

void EngineWindow::EngineRender()
{
	mRenderer->StartDraw();
	//ImGuiの描画
	GUIWinMain::RenderImGui();
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
	GUIWinMain::ShutdownImGui();
	SDL_Quit();
}
