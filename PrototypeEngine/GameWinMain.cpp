#include "GameWinMain.h"

// �����Œ�`�i�����l���w�肵�Ă�OK�j
GameState GameStateClass::mGameState = GameState::GamePlay;

bool GameStateClass::mGameEventFrag = false;

bool GameStateClass::mDebugFrag = false;


Renderer* GameWinMain::mRenderer = nullptr;

GameWinMain::GameWinMain()
	:mGameApp(nullptr)
{

}

GameWinMain::~GameWinMain()
{
	GUIWinMain::ShutdownImGui();
}

bool GameWinMain::Initialize()
{
	// SDL�̏�����
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult < 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Renderer�̐���
	mRenderer = new Renderer();
	if (!mRenderer->Initialize(WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight()))
	{
		Debug::ErrorLog("Failed to initialize Renderer");
		delete mRenderer;
		mRenderer = nullptr;
		return false;
	}

	// SDL_ttf�̏�����
	if (!TTF_Init())
	{
		Debug::ErrorLog("Failed to initialize SDL_ttf");
		return false;
	}

	Time::InitializeDeltaTime();

	mGameApp = new GameApp(this);
	if (!mGameApp->Initialize())
	{
		Debug::ErrorLog("Failed to initialize GameScenes");
		return false;
	}
	//  ImGui�̏���������
	GUIWinMain::InitializeImGui(mRenderer->GetWindow(), mRenderer->GetContext());
	/*
	*/
	return true;
}

void GameWinMain::RunLoop()
{
	while (GameStateClass::mGameState != GameState::GameEnd)
	{
		Time::UpdateDeltaTime();
		//���[�h����
		mGameApp->LoadUpdate();

		//���͏���
		mGameApp->ProcessInput();
		//���W�X�V����
		mGameApp->Update();

		GUIWinMain::UpdateImGuiState();

		Render();
	}
}

void GameWinMain::Render()
{
	//ImGui�̕`��
	{
		GUIWinMain::RenderImGui();
	}
	{
		mRenderer->Draw();
	}
}

void GameWinMain::Shutdown()
{
	UnloadData();
	TTF_Quit();
	if (mRenderer)
	{
		mRenderer->Shutdown();
		delete mRenderer;
	}
	delete mGameApp;
	SDL_Quit();
}

void GameWinMain::UnloadData()
{
	mGameApp->Release();
	if (mRenderer)
	{
		mRenderer->UnloadData();
	}
}
