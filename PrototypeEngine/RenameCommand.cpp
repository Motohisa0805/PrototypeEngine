#include "RenameCommand.h"

RenameCommand::RenameCommand(ActorObject* actor, const string& newName)
	:mTargetID(actor->GetID())
	, mNewName(newName)
{
	// 変更前の名前を保存
	mOldName = actor->GetName();
}

void RenameCommand::Execute()
{
	// アクターの名前を変更
	if (mTargetID != 0)
	{
		ActorObject* actor = SceneManager::GetNowScene()->GetActorManager()->FindActorByID(mTargetID);
		if (actor) {
			actor->SetName(mNewName);
		}
	}
	//編集操作の変更を記録する
	string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
	SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
	EditorSettingsManager::SetSaveFlag(true);
}

void RenameCommand::Undo()
{
	// アクターの名前を元に戻す
	if (mTargetID != 0)
	{
		ActorObject* actor = SceneManager::GetNowScene()->GetActorManager()->FindActorByID(mTargetID);
		if (actor) {
			actor->SetName(mOldName);
		}
	}
	//編集操作の変更を記録する
	string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
	SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
	EditorSettingsManager::SetSaveFlag(true);
}

void RenameCommand::Redo()
{
	Execute();
}
