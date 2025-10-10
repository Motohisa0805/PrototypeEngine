#include "SceneManager.h"
#include "BaseScene.h"
#include "SceneSerializer.h"
#include "EditorSettingsManager.h"

BaseScene* SceneManager::mNowScene = nullptr;

bool SceneManager::loading = false;

int SceneManager::mNowSceneIndex = 0;

// ロード予約されたファイルパスを保持する変数
std::string SceneManager::mNextSceneFilePath = "";

string SceneManager::mDefaultSceneFilePath = "Assets/Scenes/TestScene01.json";

bool SceneManager::InitializeScenes()
{
	//1.起動シーンのパスを取得
	string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();

	//2.シーンオブジェクトの作成とロード
	if (!startupScenePath.empty())
	{
		//パスが有効なら、そのファイルからロードを試みる
		mNowScene = SceneSerializer::LoadScene(startupScenePath);

	}

	//ベースに最初の動的シーンを設定(空のEditorSceneを作成)
	if (mNowScene == nullptr)
	{
		mNowScene = new EditorScene();
	}
	//シーンの初期化
	if (!mNowScene->Initialize())
	{
		return false;
	}
	//Rendererに現在のシーンを設定
	EngineWindow::GetRenderer()->SetBaseScene(mNowScene);
	return true;
}

void SceneManager::LoadSceneFromFile(const string& filePath)
{
	// 既存のシーンリストに追加するのではなく、一時的にファイルパスを保持し、
	// ChangeScene() のタイミングで処理を実行します。
	mNextSceneFilePath = filePath;
	loading = true; // ChangeScene()が呼び出されるようにフラグを立てる
}

void SceneManager::ReleaseAllScenes()
{

	mNowScene->UnloadData();
	delete mNowScene;
	mNowScene = nullptr;
}

void SceneManager::ChangeScene()
{
	//ファイルのパスがセットされている場合
	if (!mNextSceneFilePath.empty())
	{
		if (mNowScene)
		{
			EngineWindow::GetRenderer()->UnloadData();
			mNowScene->UnloadData();

			//最重要：古いシーンのメモリ解放
			delete mNowScene;
			mNowScene = nullptr;
		}

		mNowScene = SceneSerializer::LoadScene(mNextSceneFilePath);
		mNowScene->Initialize();
		EngineWindow::GetRenderer()->SetBaseScene(mNowScene);
		//...(後続の処理)...
	}
	loading = false;
}

void SceneManager::SetCurrentEditorSceneFilePath(const string& path)
{
	mDefaultSceneFilePath = path;
}
