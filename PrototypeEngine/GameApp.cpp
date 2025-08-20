#include "GameApp.h"

#include "TitleScene.h"
#include "DebugScene01.h"
#include "DebugScene02.h"


BaseScene* GameApp::mActiveScene = nullptr;

GameApp::GameApp(GameWinMain* main) 
	:mWinMain(main)
{

}

bool GameApp::Initialize()
{
	//シーン生成
	mTitleScene = new TitleScene();
	mDebugScene01 = new DebugScene01();
	mDebugScene02 = new DebugScene02();
	//シーンをリストに追加
	SceneManager::AddSceneList(mTitleScene);
	SceneManager::AddSceneList(mDebugScene01);
	SceneManager::AddSceneList(mDebugScene02);
	//ベースに最初のシーンを設定
	mActiveScene = mTitleScene;
	//シーンの初期化
	if (!mActiveScene->Initialize())
	{
		return false;
	}
	//ゲームの状態を設定
	GameStateClass::SetGameState(GameState::GamePlay);
	//Rendererに現在のシーンを設定
	GameWinMain::GetRenderer()->SetBaseScene(mActiveScene);
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
	mActiveScene->InputUpdate(state);
	InputSystem::PrepareForUpdate();
	return true;
}

bool GameApp::LoadUpdate()
{
	//ロードフラグがtrueなら
	if (SceneManager::IsLoading())
	{
		//現在のシーンのオブジェクト、画像などをアンロード
		mActiveScene->UnloadData();
		//Rendererのものもアンロード
		mWinMain->GetRenderer()->UnloadData();
		//シーンを変更
		mActiveScene = SceneManager::GetNowScene();
		GameStateClass::SetGameState(GameState::GamePlay);
		//staticも変更
		//新しいシーンの初期化
		mActiveScene->Initialize();
		//Rendererのシーンも変更
		mWinMain->GetRenderer()->SetBaseScene(mActiveScene);
		//ロードフラグを解除
		SceneManager::DisabledLoading();
	}
	return true;
}

bool GameApp::Update()
{
	mActiveScene->FixedUpdate();
	mActiveScene->Update();
	return true;
}

bool GameApp::Release()
{
	mActiveScene->UnloadData();
	SceneManager::ReleaseAllScenes();
	InputSystem::Shutdown();
	return true;
}