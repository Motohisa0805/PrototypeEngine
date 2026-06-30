#include "DeleteCommand.h"

DeleteCommand::DeleteCommand(Entity* actor)
    : mTargetID(actor->GetID()), mTarget(nullptr), mIsActiveInScene(true)
{
}

DeleteCommand::~DeleteCommand()
{
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

void DeleteCommand::Execute()
{
    if (!mIsActiveInScene)
        return;

    ActorManager* actorManager =
        SceneManager::GetCurrentRunScene()->GetActorManager();
    UIActorManager* uiActorManager =
        SceneManager::GetCurrentRunScene()->GetUIActorManager();
    if (auto actorPtr =
            dynamic_cast<ActorObject*>(actorManager->FindActorByID(mTargetID)))
    {

        // 初回実行時はコンストラクタから貰った mTarget をそのまま使う。
        // もしシーンリロードなどでポインタが変わっている（mTargetがnullptrの）場合は、IDから最新のポインタを再解決する。
        if (!mTarget)
        {
            mTarget = actorPtr;
        }

        if (actorPtr)
        {
            actorManager->DetachActor(actorPtr);
            mIsActiveInScene = false; // 「今は消えている」状態にする
        }
    }
    // UIActorか確認
    else if (auto uiActorPtr = dynamic_cast<UIActorObject*>(
                 uiActorManager->FindActorByID(mTargetID)))
    {
        // 初回実行時はコンストラクタから貰った mTarget をそのまま使う。
        // もしシーンリロードなどでポインタが変わっている（mTargetがnullptrの）場合は、IDから最新のポインタを再解決する。
        if (!mTarget)
        {
            mTarget = uiActorPtr;
        }

        if (uiActorPtr)
        {
            uiActorManager->DetachActor(uiActorPtr);
            mIsActiveInScene = false; // 「今は消えている」状態にする
        }
    }

    // 削除されたアクターが選択されていたら、安全に選択解除する
    if (SelectionManager::GetSelectedActor() == mTarget)
    {
        SelectionManager::SetSelectedActor(nullptr);
    }
}

void DeleteCommand::Undo()
{
    // 安全ガード：すでにシーンにいる、または戻すターゲットがないなら何もしない
    if (mIsActiveInScene || !mTarget)
        return;

    if (auto actorPtr = dynamic_cast<ActorObject*>(mTarget))
    {
        ActorManager* actorManager =
            SceneManager::GetCurrentRunScene()->GetActorManager();

        actorManager->ReAddActor(actorPtr);
    }
    // UIActorか確認
    else if (auto uiActorPtr = dynamic_cast<UIActorObject*>(mTarget))
    {
        UIActorManager* uiActorManager =
            SceneManager::GetCurrentRunScene()->GetUIActorManager();

        uiActorManager->ReAddActor(uiActorPtr);
    }

    mIsActiveInScene = true; // 「今は存在する」状態に戻す

    // 復元されたアクターを自動的に再選択する
    SelectionManager::SetSelectedActor(mTarget);

    mTarget = nullptr;
}

void DeleteCommand::Redo() { Execute(); }
