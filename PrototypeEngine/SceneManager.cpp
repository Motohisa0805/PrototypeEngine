#include "SceneManager.h"
#include "BaseScene.h"
#include "EditorSettingsManager.h"
#include "EngineWindow.h"
#include "GUIEditorManager.h"
#include "Renderer.h"
#include "SceneSerializer.h"

BaseScene* SceneManager::mCurrentRunScene = nullptr;

bool SceneManager::mIsLoading = false;

// ロード予約されたファイルパスを保持する変数
std::string SceneManager::mNextSceneFilePath = "";

bool SceneManager::InitializeScenes()
{
    // 1.起動シーンのパスを取得
    string startupScenePath =
        EditorSettingsManager::GetInstance().GetLastOpenedScene();

    // 2.シーンオブジェクトの作成とロード
    if (!startupScenePath.empty())
    {
        // パスが有効なら、そのファイルからロードを試みる
        mCurrentRunScene = SceneSerializer::LoadScene(startupScenePath, true);
    }

    // ベースに最初の動的シーンを設定(空のEditorSceneを作成)
    if (mCurrentRunScene == nullptr)
    {
        mCurrentRunScene = new EditorScene();
    }
    // シーンの初期化
    if (!mCurrentRunScene->Initialize())
    {
        return false;
    }
    // Rendererに現在のシーンを設定
    EngineWindow::GetRenderer()->SetBaseScene(mCurrentRunScene);
    return true;
}

void SceneManager::LoadSceneGUI(const string& filePath)
{
    // 既存のシーンリストに追加するのではなく、一時的にファイルパスを保持し、
    // ChangeScene() のタイミングで処理を実行します。
    mNextSceneFilePath = filePath;
    mIsLoading = true; // ChangeScene()が呼び出されるようにフラグを立てる
    EditorSettingsManager::GetInstance().SetLastOpenedScene(filePath);
}

void SceneManager::ReleaseAllScenes()
{
    SceneSerializer::RelaseEditorData();
    mCurrentRunScene->UnloadData();
    delete mCurrentRunScene;
    mCurrentRunScene = nullptr;
}

void SceneManager::ChangeScene()
{
    // ファイルのパスがセットされている場合
    if (!mNextSceneFilePath.empty())
    {
        if (mCurrentRunScene)
        {
            GUIEditorManager::ResetPointer();
            EngineWindow::GetRenderer()->UnloadData();
            mCurrentRunScene->UnloadData();
            // 最重要：古いシーンのメモリ解放
            delete mCurrentRunScene;
            mCurrentRunScene = nullptr;
        }

        mCurrentRunScene = SceneSerializer::LoadScene(mNextSceneFilePath, true);
        mCurrentRunScene->Initialize();
        EngineWindow::GetRenderer()->SetBaseScene(mCurrentRunScene);
        if (GUIEditorManager::IsPlaying())
        {
            // 実行中なら静的バッチの構築も行う
            EngineWindow::GetRenderer()->BuildStaticBatch();
        }
    }
    mIsLoading = false;
}

void SceneManager::GamePlayEndInitilaizeScene()
{
    if (mCurrentRunScene)
    {
        GUIEditorManager::ResetPointer();
        EngineWindow::GetRenderer()->UnloadData();
        mCurrentRunScene->UnloadData();
        // 最重要：古いシーンのメモリ解放
        delete mCurrentRunScene;
        mCurrentRunScene = nullptr;
    }
    // 実行終了時のシーン情報
    mCurrentRunScene = SceneSerializer::LoadScene(
        SceneSerializer::GetTempEditingPath().string());
    mCurrentRunScene->Initialize();
    EngineWindow::GetRenderer()->SetBaseScene(mCurrentRunScene);
    if (GUIEditorManager::IsPlaying())
    {
        // 実行中なら静的バッチの構築も行う
        EngineWindow::GetRenderer()->BuildStaticBatch();
    }
}

const vector<ActorObject*>& SceneManager::GetAllActorsInCurrentScene()
{
    if (mCurrentRunScene)
    {
        return mCurrentRunScene->GetActorManager()->GetActors();
    }
    return vector<ActorObject*>{};
}
