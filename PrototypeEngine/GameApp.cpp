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
			GameStateClass::SetGameState(GameState::GameEnd);
			break;
		}
	}

	//入力更新
	SceneManager::GetNowScene()->InputUpdate(state);
	InputSystem::PrepareForUpdate();
	return true;
}

bool GameApp::ProcessInput2()
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
		Update();
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