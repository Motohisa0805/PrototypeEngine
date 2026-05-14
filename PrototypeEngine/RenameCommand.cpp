#include "RenameCommand.h"

RenameCommand::RenameCommand(ActorObject* actor, const string& newName)
	:mTarget(actor)
	, mNewName(newName)
{
	// 変更前の名前を保存
	mOldName = actor->GetName();
}

void RenameCommand::Execute()
{
	// アクターの名前を変更
	if (mTarget)
	{
		mTarget->SetName(mNewName);
	}
	//編集操作の変更を記録する
	string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
	SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
	EditorSettingsManager::SetSaveFlag(true);
}

void RenameCommand::Undo()
{
	// アクターの名前を元に戻す
	if (mTarget)
	{
		mTarget->SetName(mOldName);
	}
	//編集操作の変更を記録する
	string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
	SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
	EditorSettingsManager::SetSaveFlag(true);
}

void RenameCommand::Redo()
{
	// アクターの名前を再度変更
	if (mTarget)
	{
		mTarget->SetName(mNewName);
	}
	//編集操作の変更を記録する
	string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
	SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
	EditorSettingsManager::SetSaveFlag(true);
}
