#include "GameWinMain.h"

// ここで定義（初期値を指定してもOK）
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
	//入力処理
	mGameApp->ProcessInput2();
}

void GameWinMain::RunLoop()
{
	while (GameStateClass::mGameState != GameState::GameEnd)
	{
		Time::UpdateDeltaTime();
		//ロード処理
		mGameApp->LoadUpdate();
		//座標更新処理
		mGameApp->Update();
	}
}

void GameWinMain::GameRunLoop()
{
	//ロード処理
	mGameApp->LoadUpdate();

	//座標更新処理
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
