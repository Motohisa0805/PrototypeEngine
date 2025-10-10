#include "GameWinMain.h"

// �����Œ�`�i�����l���w�肵�Ă�OK�j
GameState GameStateClass::mGameState = GameState::GamePlay;

bool GameStateClass::mGameEventFrag = false;

bool GameStateClass::mDebugFrag = false;


//Renderer* GameWinMain::mRenderer = nullptr;

GameWinMain::GameWinMain()
	:mGameApp(nullptr)
{

}

GameWinMain::~GameWinMain()
{
	
}

bool GameWinMain::Initialize()
{
	

	mGameApp = new GameApp(this);
	if (!mGameApp->Initialize())
	{
		Debug::ErrorLog("Failed to initialize GameScenes");
		return false;
	}
	return true;
}

void GameWinMain::InputUpdate()
{
	//���͏���
	mGameApp->ProcessInput2();
}

void GameWinMain::RunLoop()
{
	while (GameStateClass::mGameState != GameState::GameEnd)
	{
		Time::UpdateDeltaTime();
		//���[�h����
		mGameApp->LoadUpdate();
		//���W�X�V����
		mGameApp->Update();
	}
}

void GameWinMain::GameRunLoop()
{
	//���[�h����
	mGameApp->LoadUpdate();

	//���W�X�V����
	mGameApp->Update();
}

void GameWinMain::LoadGame_Engine()
{
	mGameApp->LoadUpdate();
}

void GameWinMain::Shutdown()
{
	mGameApp->Release();
	delete mGameApp;
	mGameApp = nullptr;
}

void GameWinMain::UnloadData()
{
	mGameApp->Release();
}
