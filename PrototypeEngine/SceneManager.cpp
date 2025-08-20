#include "SceneManager.h"
#include "BaseScene.h"

#include "TitleScene.h"
#include "DebugScene01.h"
#include "DebugScene02.h"

std::unordered_map<int,BaseScene*> SceneManager::mScenes;

BaseScene* SceneManager::mNowScene;

bool SceneManager::loading = false;

int SceneManager::mNowSceneIndex = 0;

TitleScene* SceneManager::mTitleScene = nullptr;
DebugScene01* SceneManager::mDebugScene01 = nullptr;
DebugScene02* SceneManager::mDebugScene02 = nullptr;

bool SceneManager::InitializeScenes()
{
	//シーン生成
	mTitleScene = new TitleScene();
	mDebugScene01 = new DebugScene01();
	mDebugScene02 = new DebugScene02();
	//シーンをリストに追加
	AddSceneList(mTitleScene);
	AddSceneList(mDebugScene01);
	AddSceneList(mDebugScene02);
	//ベースに最初のシーンを設定
	mNowScene = mTitleScene;
	//シーンの初期化
	if (!mNowScene->Initialize())
	{
		return false;
	}
	//Rendererに現在のシーンを設定
	GameWinMain::GetRenderer()->SetBaseScene(mNowScene);
	return true;
}

void SceneManager::LoadScene(int index)
{
	if (mNowScene == mScenes[index]) { return; }
	mNowScene = mScenes[index];
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
