#include "DeleteCommand.h"

DeleteCommand::DeleteCommand(ActorObject* actor)
	: mTargetID(actor->GetID())
	, mTarget(nullptr)
	, mIsActiveInScene(true)
{
}

DeleteCommand::~DeleteCommand()
{
	if (!mIsActiveInScene && mTarget)
	{
		delete mTarget;
	}
}

void DeleteCommand::Execute()
{
	if (!mIsActiveInScene) return;

	ActorManager* actorManager = SceneManager::GetNowScene()->GetActorManager();
	
	// 初回実行時はコンストラクタから貰った mTarget をそのまま使う。
	// もしシーンリロードなどでポインタが変わっている（mTargetがnullptrの）場合は、IDから最新のポインタを再解決する。
	if (!mTarget)
	{
		mTarget = actorManager->FindActorByID(mTargetID);
	}
	
	if (mTarget)
	{
		actorManager->DetachActor(mTarget);
		mIsActiveInScene = false; // 「今は消えている」状態にする
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
	if (mIsActiveInScene || !mTarget) return;

	ActorManager* actorManager = SceneManager::GetNowScene()->GetActorManager();

	actorManager->ReAddActor(mTarget);

	mIsActiveInScene = true; // 「今は存在する」状態に戻す

	// 復元されたアクターを自動的に再選択する
	SelectionManager::SetSelectedActor(mTarget);

	mTarget = nullptr;
}

void DeleteCommand::Redo()
{
	Execute();
}
