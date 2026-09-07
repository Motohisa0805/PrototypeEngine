#include "DeleteCommand.h"

void DeleteCommand::CollectTargets(Entity* actor) 
{
    if(!actor)return;

    mTargets.push_back({actor->GetID(), actor});

    //子アクターの再帰的に取得
    if (auto actorPtr = dynamic_cast<ActorObject*>(actor))
    {
        auto& children = actorPtr->GetTransform()->GetChildActorList();
        for (auto child : children)
        {
            CollectTargets(child);
        }
    }
    else if (auto uiActorptr = dynamic_cast<UIActorObject*>(actor))
    {
        auto& children = uiActorptr->GetRectTransform()->GetChildActorList();
        for (auto child : children)
        {
            CollectTargets(child);
        }
    }
}

DeleteCommand::DeleteCommand(Entity* actor)
    : mIsActiveInScene(true)
{
    CollectTargets(actor);
}

DeleteCommand::~DeleteCommand()
{
    if (!mIsActiveInScene)
    {
        for (auto it = mTargets.rbegin(); it != mTargets.rend(); ++it)
        {
            if (it->ptr)
            {
                if (auto actorPtr = dynamic_cast<ActorObject*>(it->ptr))
                {
                    delete actorPtr;
                }
                // UIActorか確認
                else if (auto uiActorPtr = dynamic_cast<UIActorObject*>(it->ptr))
                {
                    delete uiActorPtr;
                }
            }
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
    
    for (auto& targetData : mTargets)
    {
        if (auto actorPtr = dynamic_cast<ActorObject*>(
                actorManager->FindActorByID(targetData.sID)))
        {

            // 初回実行時はコンストラクタから貰った mTarget をそのまま使う。
            // もしシーンリロードなどでポインタが変わっている（mTargetがnullptrの）場合は、IDから最新のポインタを再解決する。
            if (!targetData.ptr)
            {
                targetData.ptr = actorPtr;
            }

            if (actorPtr)
            {
                actorManager->DetachActor(actorPtr);
                mIsActiveInScene = false; // 「今は消えている」状態にする
            }
        }
        // UIActorか確認
        else if (auto uiActorPtr = dynamic_cast<UIActorObject*>(
                     uiActorManager->FindActorByID(targetData.sID)))
        {
            // 初回実行時はコンストラクタから貰った mTarget をそのまま使う。
            // もしシーンリロードなどでポインタが変わっている（mTargetがnullptrの）場合は、IDから最新のポインタを再解決する。
            if (!targetData.ptr)
            {
                targetData.ptr = uiActorPtr;
            }

            if (uiActorPtr)
            {
                uiActorManager->DetachActor(uiActorPtr);
                mIsActiveInScene = false; // 「今は消えている」状態にする
            }
        }

        // 削除されたアクターが選択されていたら、安全に選択解除する
        if (SelectionManager::GetSelectedActor() == targetData.ptr)
        {
            SelectionManager::SetSelectedActor(nullptr);
        }
    }
    mIsActiveInScene = false;
}

void DeleteCommand::Undo()
{
    // 安全ガード：すでにシーンにいる、または戻すターゲットがないなら何もしない
    if (mIsActiveInScene)
        return;

    ActorManager* actorManager = SceneManager::GetCurrentRunScene()->GetActorManager();
    UIActorManager* uiActorManager = SceneManager::GetCurrentRunScene()->GetUIActorManager();
    
    for (auto& targetData : mTargets)
    {
        if (!targetData.ptr)continue;

        if (auto actorPtr = dynamic_cast<ActorObject*>(targetData.ptr))
        {

            actorManager->ReAddActor(actorPtr);
        }
        // UIActorか確認
        else if (auto uiActorPtr = dynamic_cast<UIActorObject*>(targetData.ptr))
        {

            uiActorManager->ReAddActor(uiActorPtr);
        }
    }

    mIsActiveInScene = true; // 「今は存在する」状態に戻す

    if (!mTargets.empty() && mTargets[0].ptr)
    {
        // 復元されたアクターを自動的に再選択する
        SelectionManager::SetSelectedActor(mTargets[0].ptr);
    }
}

void DeleteCommand::Redo() { Execute(); }
