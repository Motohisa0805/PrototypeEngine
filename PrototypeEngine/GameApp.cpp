#include "GameApp.h"

#include "TitleScene.h"
#include "DebugScene01.h"
#include "DebugScene02.h"


GameApp::GameApp(GameWinMain* main) 
	:mWinMain(main)
{

}

bool GameApp::Initialize()
{
	//ゲームの状態を設定
	GameStateClass::SetGameState(GameState::GamePlay);
	return true;
}

bool GameApp::ProcessInput()
{
	const InputState& state = InputSystem::GetState();
	//入力更新
	SceneManager::GetNowScene()->InputUpdate(state);
	return true;
}

bool GameApp::LoadUpdate()
{
	//ロードフラグがtrueなら
	if (SceneManager::IsLoading())
	{
		SceneManager::ChangeScene();
	}
	return true;
}

bool GameApp::Update()
{
	SceneManager::GetNowScene()->FixedUpdate();
	SceneManager::GetNowScene()->Update();
	return true;
}

bool GameApp::Release()
{
	//ゲームシーンの解放
	SceneManager::GetNowScene()->UnloadData();
	//入力システムのシャットダウン
	InputSystem::Shutdown();
	return true;
}