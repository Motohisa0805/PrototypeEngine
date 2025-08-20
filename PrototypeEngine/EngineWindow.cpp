#include "EngineWindow.h"

EngineState EngineWindow::mEngineState = EngineState::None;

EngineWindow::EngineWindow()
	: mRenderer(nullptr)
{
	GUIWinMain::ShutdownImGui();
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
	/*
	// Rendererの生成
	mRenderer = new Renderer();
	if (!mRenderer->Initialize(WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight()))
	{
		Debug::ErrorLog("Failed to initialize Renderer");
		delete mRenderer;
		mRenderer = nullptr;
		return false;
	}
	*/

	// SDL_ttfの初期化
	if (!TTF_Init())
	{
		Debug::ErrorLog("Failed to initialize SDL_ttf");
		return false;
	}

	Time::InitializeDeltaTime();

	EngineWindow::mEngineState = EngineState::Run;

	//  ImGuiの初期化処理
	GUIWinMain::InitializeImGui(mRenderer->GetWindow(), mRenderer->GetContext());
	
	//起動時に最初のシーンを初期化
	SceneManager::InitializeScenes();

	mGameWindow = new GameWinMain();
	mGameWindow->Initialize();
	
	return true;
}

//エンジンプロジェクトのループ処理
void EngineWindow::EngineRunLoop()
{
	while (EngineWindow::mEngineState != EngineState::Run)
	{
		Time::UpdateDeltaTime();

		GUIWinMain::UpdateImGuiState();

		EngineRender();

		//ここからゲーム内の更新開始
		if (GUIWinMain::IsPlaying)
		{

			mGameWindow->RunLoop();
		}

	}
}

void EngineWindow::EngineRender()
{
	//ImGuiの描画
	{
		GUIWinMain::RenderImGui();
	}
	{
		
	}
}

void EngineWindow::EngineUnloadData()
{
}

void EngineWindow::EngineShutdown()
{
	EngineUnloadData();
	TTF_Quit();
	if (mRenderer)
	{
		mRenderer->Shutdown();
		delete mRenderer;
	}
	SDL_Quit();
}
