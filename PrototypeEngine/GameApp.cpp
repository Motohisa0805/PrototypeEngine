#include "GameApp.h"
#include "SceneManager.h"
#include "BaseScene.h"


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
	SceneManager::GetCurrentRunScene()->InputUpdate(state);
	return true;
}

bool GameApp::LoadUpdate()
{
	//ロードフラグがtrueなら
	if (SceneManager::IsLoading())
	{
		SceneManager::ChangeScene();
		return true;
	}
	return false;
}

bool GameApp::Update()
{
	SceneManager::GetCurrentRunScene()->FixedUpdate();
	SceneManager::GetCurrentRunScene()->Update();
	return true;
}

bool GameApp::Release()
{
	//ゲームシーンの解放
	SceneManager::GetCurrentRunScene()->UnloadData();
	//入力システムのシャットダウン
	InputSystem::Shutdown();
	return true;
}