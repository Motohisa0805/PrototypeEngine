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
    UIActorManager* uiActorManager = SceneManager::GetNowScene()->GetUIActorManager();
    if (mTargetID == 0)
    {
        // 1. 完全なる初回実行時：クリップボードからさらに複製して生成する
        if (EditorClipboard::HasCopiedActor())
        {
            if (auto actorPtr = dynamic_cast<ActorObject*>(actorManager->FindActorByID(EditorClipboard::GetCopiedActor()))) {
                mTarget = actorPtr->Clone();
                
            }
            // UIActorか確認
            else if (auto uiActorPtr = dynamic_cast<UIActorObject*>(uiActorManager->FindActorByID(EditorClipboard::GetCopiedActor()))) {
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
            if (auto actorPtr = dynamic_cast<ActorObject*>(mTarget)) {
                actorManager->AddActor(actorPtr);
            }
            // UIActorか確認
            else if (auto uiActorPtr = dynamic_cast<UIActorObject*>(mTarget)) {
                uiActorManager->AddActor(uiActorPtr);
            }
            mTarget = nullptr;
            mIsActiveInScene = true;
        }
    }

    // 実行完了後、IDから最新のオブジェクトを検索して選択状態にする
    if (mTargetID != 0)
    {
        ActorObject* currentActor = actorManager->FindActorByID(mTargetID);
        UIActorObject* currentUIActor = uiActorManager->FindActorByID(mTargetID);
        if (currentActor)
        {
            SelectionManager::SetSelectedActor(currentActor);
        }
        else if (currentUIActor) {
            SelectionManager::SetSelectedActor(currentActor);
        }
    }
}

void PasteActorCommand::Undo()
{
    if (mTargetID == 0 || !mIsActiveInScene) return;

    ActorManager* actorManager = SceneManager::GetNowScene()->GetActorManager();
    UIActorManager* uiActorManager = SceneManager::GetNowScene()->GetUIActorManager();

    // シーンに生きている最新のポインタをIDから取得
    ActorObject* currentActor = actorManager->FindActorByID(mTargetID);
    UIActorObject* currentUIActor = uiActorManager->FindActorByID(mTargetID);
    if (currentActor)
    {
        // シーンのリストから除外する（メモリは delete しない）
        actorManager->RemoveActor(currentActor);

        mTarget = currentActor;
    }
    else if (currentUIActor) {
        // シーンのリストから除外する（メモリは delete しない）
        uiActorManager->RemoveActor(currentUIActor);

        mTarget = currentUIActor;
    }

    mIsActiveInScene = false;

    // 選択解除処理
    if (SelectionManager::GetSelectedActor() == currentActor || SelectionManager::GetSelectedActor() == currentUIActor || SelectionManager::GetSelectedActor() == mTarget)
    {
        SelectionManager::SetSelectedActor(nullptr);
    }
}

void PasteActorCommand::Redo()
{
    Execute();
}
