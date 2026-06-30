#include "DuplicateCommand.h"

DuplicateCommand::DuplicateCommand()
    : mTargetID(0)
    , mTarget(nullptr) // 一時的にアクターをホールドするポインタ
    , mIsActiveInScene(false)
{
}

DuplicateCommand::~DuplicateCommand()
{
    // メモリを解放（メモリリーク防止）
    if (!mIsActiveInScene && mTarget)
    {
        if (auto actorPtr = dynamic_cast<ActorObject*>(mTarget))
        {
            delete actorPtr;
        }
        // UIActorか確認
        else if (auto uiActorPtr = dynamic_cast<UIActorObject*>(mTarget))
        {
            delete uiActorPtr;
        }
    }
}

void DuplicateCommand::Execute()
{
    ActorManager* actorManager =
        SceneManager::GetCurrentRunScene()->GetActorManager();
    UIActorManager* uiActorManager =
        SceneManager::GetCurrentRunScene()->GetUIActorManager();
    if (mTargetID == 0)
    {
        // 1. 完全なる初回実行時：クリップボードからさらに複製して生成する
        if (SelectionManager::GetSelectedActor())
        {
            Entity* selectedActor = SelectionManager::GetSelectedActor();
            if (auto actorPtr = dynamic_cast<ActorObject*>(
                    actorManager->FindActorByID(selectedActor->GetID())))
            {
                mTarget = actorPtr->Clone();
            }
            // UIActorか確認
            else if (auto uiActorPtr = dynamic_cast<UIActorObject*>(
                         uiActorManager->FindActorByID(selectedActor->GetID())))
            {
                mTarget = uiActorPtr->Clone();
            }
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
            if (auto actorPtr = dynamic_cast<ActorObject*>(mTarget))
            {
                actorManager->AddActor(actorPtr);
            }
            // UIActorか確認
            else if (auto uiActorPtr = dynamic_cast<UIActorObject*>(mTarget))
            {
                uiActorManager->AddActor(uiActorPtr);
            }
            mTarget          = nullptr;
            mIsActiveInScene = true;
        }
    }

    // 実行完了後、IDから最新のオブジェクトを検索して選択状態にする
    if (mTargetID != 0)
    {
        ActorObject*   currentActor = actorManager->FindActorByID(mTargetID);
        UIActorObject* currentUIActor =
            uiActorManager->FindActorByID(mTargetID);
        if (currentActor)
        {
            SelectionManager::SetSelectedActor(currentActor);
        }
        else if (currentUIActor)
        {
            SelectionManager::SetSelectedActor(currentUIActor);
        }
    }
}

void DuplicateCommand::Undo()
{
    if (mTargetID == 0 || !mIsActiveInScene)
        return;

    ActorManager* actorManager =
        SceneManager::GetCurrentRunScene()->GetActorManager();
    UIActorManager* uiActorManager =
        SceneManager::GetCurrentRunScene()->GetUIActorManager();

    // シーンに生きている最新のポインタをIDから取得
    ActorObject*   currentActor   = actorManager->FindActorByID(mTargetID);
    UIActorObject* currentUIActor = uiActorManager->FindActorByID(mTargetID);
    if (currentActor)
    {
        // シーンのリストから除外する（メモリは delete しない）
        actorManager->RemoveActor(currentActor);

        mTarget = currentActor;
    }
    else if (currentUIActor)
    {
        // シーンのリストから除外する（メモリは delete しない）
        uiActorManager->RemoveActor(currentUIActor);

        mTarget = currentUIActor;
    }

    mIsActiveInScene = false;

    // 選択を複製前の状態に戻す（安全に選択解除する）
    SelectionManager::SetSelectedActor(mTarget);
}

void DuplicateCommand::Redo() { Execute(); }
