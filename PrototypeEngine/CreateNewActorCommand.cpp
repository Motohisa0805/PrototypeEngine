#include "CreateNewActorCommand.h"

CreateNewActorCommand::CreateNewActorCommand()
    : mTargetID(0)
    , mTarget(nullptr)
    , mIsActiveInScene(false)
{
}

CreateNewActorCommand::~CreateNewActorCommand()
{
    if (!mIsActiveInScene && mTarget)
    {
        delete mTarget;
    }
}

void CreateNewActorCommand::Execute()
{
    ActorManager* actorManager = SceneManager::GetNowScene()->GetActorManager();

    if (mTargetID == 0)
    {
        // 1. 完全なる初回実行時：新しくアクターを生成してシーンに登録する
        mTarget = new ActorObject();
        mTargetID = mTarget->GetID();

        // シーンに所有権を渡したため、コマンド側のポインタは安全にクリアする
        mTarget = nullptr;
        mIsActiveInScene = true;
    }
    else
    {
        // 2. Redo（再実行）時
        // Undo時にコマンド側（mTarget）に回収しておいたインスタンスを、もう一度シーンに戻す
        if (!mIsActiveInScene && mTarget)
        {
            actorManager->AddActor(mTarget);
            mTarget = nullptr; // 所有権を再度シーンに渡す
            mIsActiveInScene = true;
        }
    }

    // 生成・復元された最新のオブジェクトをIDから解決して選択状態にする
    if (mTargetID != 0)
    {
        ActorObject* currentActor = actorManager->FindActorByID(mTargetID);
        if (currentActor)
        {
            SelectionManager::SetSelectedActor(currentActor);
        }
    }

    // 編集操作の変更を記録する
    string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
    SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene()); 
    EditorSettingsManager::SetSaveFlag(true); 
}

void CreateNewActorCommand::Undo()
{
    // 安全ガード
    if (mTargetID == 0 || !mIsActiveInScene) return;

    ActorManager* actorManager = SceneManager::GetNowScene()->GetActorManager();

    // 「その瞬間」にシーンに存在しているポインタをIDから検索
    ActorObject* currentActor = actorManager->FindActorByID(mTargetID);

    if (currentActor)
    {
        // シーンからアクターを除外
        actorManager->RemoveActor(currentActor);

        mTarget = currentActor;
    }

    mIsActiveInScene = false;

    // もし現在生成したアクターが選択されていたら、安全に解除
    if (SelectionManager::GetSelectedActor() == currentActor || SelectionManager::GetSelectedActor() == mTarget)
    {
        SelectionManager::SetSelectedActor(nullptr);
    }

    // 編集操作の変更を記録する
    string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
    SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene()); 
    EditorSettingsManager::SetSaveFlag(true);
}

void CreateNewActorCommand::Redo()
{
	Execute();
}
