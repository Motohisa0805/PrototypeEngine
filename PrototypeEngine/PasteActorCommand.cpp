#include "PasteActorCommand.h"

PasteActorCommand::PasteActorCommand()
    : mTargetID(0)       // ヘッダーに uint64_t mTargetID; を追加してください
    , mTarget(nullptr)   // 一時的にアクターをホールドするポインタ
    , mIsActiveInScene(false) //
{
}

PasteActorCommand::~PasteActorCommand()
{
    // Undoされたまま（シーンに存在しない状態のまま）履歴から消えたら、
    // コマンドが責任を持ってメモリを解放する（メモリリーク防止）
    if (!mIsActiveInScene && mTarget)
    {
        delete mTarget;
    }
}

void PasteActorCommand::Execute()
{
    ActorManager* actorManager = SceneManager::GetNowScene()->GetActorManager();

    if (mTargetID == 0)
    {
        // 1. 完全なる初回実行時：クリップボードからさらに複製して生成する
        if (EditorClipboard::HasCopiedActor())
        {
            mTarget = actorManager->FindActorByID(EditorClipboard::GetCopiedActor())->Clone();
            mTarget->SetName(mTarget->GetName() + " (Copy)");

            // 生成された新しいアクターのユニークIDをコマンドに記憶する
            mTargetID = mTarget->GetID();

            // 所有権をシーン側に渡したため、コマンド側のポインタはクリアする
            mIsActiveInScene = true;
        }
    }
    else
    {
        // 2. Redo（再実行）時
        if (!mIsActiveInScene && mTarget)
        {
            // コマンドが安全にホールドしていたインスタンスをシーンに戻す
            actorManager->AddActor(mTarget);
            mTarget = nullptr;
            mIsActiveInScene = true;
        }
    }

    // 実行完了後、IDから最新のオブジェクトを検索して選択状態にする
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

void PasteActorCommand::Undo()
{
    if (mTargetID == 0 || !mIsActiveInScene) return;

    ActorManager* actorManager = SceneManager::GetNowScene()->GetActorManager();

    // シーンに生きている最新のポインタをIDから取得
    ActorObject* currentActor = actorManager->FindActorByID(mTargetID);

    if (currentActor)
    {
        // シーンのリストから除外する（メモリは delete しない）
        actorManager->RemoveActor(currentActor);

        mTarget = currentActor;
    }

    mIsActiveInScene = false;

    // 選択解除処理
    if (SelectionManager::GetSelectedActor() == currentActor || SelectionManager::GetSelectedActor() == mTarget)
    {
        SelectionManager::SetSelectedActor(nullptr);
    }

    // 編集操作の変更を記録する
    string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
    SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
    EditorSettingsManager::SetSaveFlag(true);
}

void PasteActorCommand::Redo()
{
    Execute();
}
