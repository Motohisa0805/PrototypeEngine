#include "ReparentAndReorderCommand.h"

ReparentAndReorderCommand::ReparentAndReorderCommand(ActorObject* target, ActorObject* newParent, size_t toIndex)
	:mTarget(target)
	,mNewParent(newParent)
	,mToIndex(toIndex)
{
	//コンストラクタで「移動前」の状態を自動保存する
	mOldParent = target->GetTransform()->GetParentActor();

	auto& oldList = mOldParent ? mOldParent->GetTransform()->GetChildActorListMutable()
		: SceneManager::GetNowScene()->GetActorManager()->GetActorsMutable();

	auto it = std::find(oldList.begin(), oldList.end(), target);
	mFromIndex = std::distance(oldList.begin(), it);
}

void ReparentAndReorderCommand::Execute()
{
	// 1. 親を変更する（SetParent の内部で古い親のリストから外れ、新しい親のリストの「末尾」に追加されると仮定）
	mTarget->GetTransform()->SetParent(mNewParent);

	// 2. 新しいリストの末尾（size - 1）から、指定された挿入位置（mToIndex）まで移動させる
	auto& newList = mNewParent ? mNewParent->GetTransform()->GetChildActorListMutable()
		: SceneManager::GetNowScene()->GetActorManager()->GetActorsMutable();

	ImGuiHelper::ReorderVector(newList, newList.size() - 1, mToIndex);

	//編集操作の変更を記録する
	string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
	SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
	EditorSettingsManager::SetSaveFlag(true);
}

void ReparentAndReorderCommand::Undo()
{
	// 元に戻すときは、逆の操作を行う
	mTarget->GetTransform()->SetParent(mOldParent);

	auto& oldList = mOldParent ? mOldParent->GetTransform()->GetChildActorListMutable()
		: SceneManager::GetNowScene()->GetActorManager()->GetActorsMutable();

	ImGuiHelper::ReorderVector(oldList, oldList.size() - 1, mFromIndex);

	//編集操作の変更を記録する
	string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
	SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
	EditorSettingsManager::SetSaveFlag(true);
}

void ReparentAndReorderCommand::Redo()
{
	Execute();
}
