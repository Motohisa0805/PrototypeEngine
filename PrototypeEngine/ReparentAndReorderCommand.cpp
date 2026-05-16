#include "ReparentAndReorderCommand.h"

ReparentAndReorderCommand::ReparentAndReorderCommand(ActorObject* target, ActorObject* newParent, size_t toIndex)
	: mTargetID(target ? target->GetID() : 0)
	, mNewParentID(newParent ? newParent->GetID() : 0)
	, mToIndex(toIndex)
	, mOldParentID(0)
	, mFromIndex(0)
{
	if (!target) return;
	// 移動前の親のIDを記憶
	ActorObject* oldParent = target->GetTransform()->GetParentActor();
	mOldParentID = oldParent ? oldParent->GetID() : 0;

	// 移動前のリストを取得し、自分のインデックス（mFromIndex）を特定しておく
	auto& oldList = GetActorListMutable(mOldParentID);
	auto it = std::find(oldList.begin(), oldList.end(), target);
	if (it != oldList.end())
	{
		mFromIndex = std::distance(oldList.begin(), it);
	}
}

void ReparentAndReorderCommand::Execute()
{
	// IDから「今シーンに存在する最新のオブジェクトポインタ」を解決する
	ActorManager* actorManager = SceneManager::GetNowScene()->GetActorManager();
	ActorObject* target = actorManager->FindActorByID(mTargetID);
	ActorObject* newParent = (mNewParentID != 0) ? actorManager->FindActorByID(mNewParentID) : nullptr;

	// 安全ガード：オブジェクトがリロードなどで消えていたら処理しない
	if (!target) return;

	// 親を変更する（SetParent）
	target->GetTransform()->SetParent(newParent);

	// 新しいリストの末尾から、指定された挿入位置まで移動させる
	auto& newList = GetActorListMutable(mNewParentID);
	ImGuiHelper::ReorderVector(newList, newList.size() - 1, mToIndex);

	// 編集操作の変更を記録する
	string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
	SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
	EditorSettingsManager::SetSaveFlag(true);
}

void ReparentAndReorderCommand::Undo()
{
	ActorManager* actorManager = SceneManager::GetNowScene()->GetActorManager();
	ActorObject* target = actorManager->FindActorByID(mTargetID);
	ActorObject* oldParent = (mOldParentID != 0) ? actorManager->FindActorByID(mOldParentID) : nullptr;

	if (!target) return;

	// 元に戻すときは、逆の操作を行う
	target->GetTransform()->SetParent(oldParent);

	auto& oldList = GetActorListMutable(mOldParentID);
	ImGuiHelper::ReorderVector(oldList, oldList.size() - 1, mFromIndex);

	// 編集操作の変更を記録する
	string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
	SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
	EditorSettingsManager::SetSaveFlag(true);
}

void ReparentAndReorderCommand::Redo()
{
	Execute();
}

vector<ActorObject*>& ReparentAndReorderCommand::GetActorListMutable(uint64_t parentID)
{
	ActorManager* actorManager = SceneManager::GetNowScene()->GetActorManager();

	if (parentID == 0) {
		// ルート階層（親なし）の場合は、ActorManager直属のリストを返す
		return actorManager->GetActorsMutable();
	}
	else {
		// 特定の親がいる場合は、その親の子供リストを返す
		ActorObject* parent = actorManager->FindActorByID(parentID);
		// 万が一親が見つからない場合は、クラッシュ防止のためルートのリストをセーフティとして返します
		if (!parent) return actorManager->GetActorsMutable();

		return parent->GetTransform()->GetChildActorListMutable();
	}
}
