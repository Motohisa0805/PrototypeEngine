#include "SceneManager.h"
#include "BaseScene.h"

#include "TitleScene.h"
#include "DebugScene01.h"
#include "DebugScene02.h"

std::unordered_map<int,BaseScene*> SceneManager::mScenes;

BaseScene* SceneManager::mNowScene = nullptr;

BaseScene* SceneManager::mNextScene = nullptr;

bool SceneManager::loading = false;

int SceneManager::mNowSceneIndex = 0;

bool SceneManager::InitializeScenes()
{
	//シーンをリストに追加
	AddSceneList(new TitleScene());
	AddSceneList(new DebugScene01());
	AddSceneList(new DebugScene02());
	//ベースに最初のシーンを設定
	mNowScene = mScenes[0];
	//シーンの初期化
	if (!mNowScene->Initialize())
	{
		return false;
	}
	//Rendererに現在のシーンを設定
	EngineWindow::GetRenderer()->SetBaseScene(mNowScene);
	return true;
}

void SceneManager::LoadScene(int index)
{
	if (mNowScene == mScenes[index]) { return; }
	mNextScene = mScenes[index];
	mNowSceneIndex = index;
	loading = true;
}

void SceneManager::AddSceneList(class BaseScene* scene)
{
	auto iter = mScenes.find(mScenes.size());
	if (iter != mScenes.end())
	{
		return;
	}
	else
	{
		mScenes.emplace(mScenes.size(), scene);
	}
}

void SceneManager::ReleaseAllScenes()
{
	for (auto& pair : mScenes)
	{
		delete pair.second;
	}
	mScenes.clear();
	mNowScene = nullptr;
}

void SceneManager::ChangeScene()
{
	if (mNextScene)
	{
		if (mNowScene)
		{
			EngineWindow::GetRenderer()->UnloadData();
			mNowScene->UnloadData();
		}
		GameStateClass::SetGameState(GameState::GamePlay);
		mNowScene = mNextScene;
		mNextScene = nullptr;
		mNowScene->Initialize();
		EngineWindow::GetRenderer()->SetBaseScene(mNowScene);
	}
	loading = false;
}
