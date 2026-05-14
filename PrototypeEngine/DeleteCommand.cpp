#include "DeleteCommand.h"

DeleteCommand::DeleteCommand(ActorObject* actor)
	: mTarget(actor)
	, mIsActiveInScene(true)
{
}

DeleteCommand::~DeleteCommand()
{
	// コマンドが破棄される時、アクターがシーンに戻っていないなら
		// ここで初めてメモリを解放する（メモリリーク防止）
	if (!mIsActiveInScene && mTarget) {
		delete mTarget;
		mTarget = nullptr;
	}
}

void DeleteCommand::Execute()
{
	if (mIsActiveInScene) {
		auto manager = SceneManager::GetNowScene()->GetActorManager();
		manager->DetachActor(mTarget); // シーンから切り離す
		mIsActiveInScene = false;      // 「今は消えている」状態
		SelectionManager::SetSelectedActor(nullptr);

		//編集操作の変更を記録する
		string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
		SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
		EditorSettingsManager::SetSaveFlag(true);
	}
}

void DeleteCommand::Undo()
{
	if (!mIsActiveInScene) {
		auto manager = SceneManager::GetNowScene()->GetActorManager();
		manager->ReAddActor(mTarget);    // シーンに戻す
		mIsActiveInScene = true;       // 「今は存在する」状態
		SelectionManager::SetSelectedActor(mTarget);

		//編集操作の変更を記録する
		string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
		SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
		EditorSettingsManager::SetSaveFlag(true);
	}
}

void DeleteCommand::Redo()
{
	Execute();
}
