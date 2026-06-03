#include "CommandManager.h"

void CommandManager::Execute(std::unique_ptr<ICommand> command)
{
	command->Execute();

	// 新しい操作が行われたらRedoスタックはクリアする（Unityと同じ挙動）
	if (!GUIWinMain::IsPlaying()) {
		Get().mRedoStack.clear();
		Get().mUndoStack.push_back(std::move(command));

		if (Get().mUndoStack.size() > 50) {
			Get().mUndoStack.erase(Get().mUndoStack.begin()); // 一番古いコマンドを削除
		}
		// 編集操作の変更を記録する
		string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
		SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
		EditorSettingsManager::SetSaveFlag(true);
	}
}

void CommandManager::NoHistoryExecute(std::unique_ptr<ICommand> command)
{
	command->NoHistoryExecute();

	// 新しい操作が行われたらRedoスタックはクリアする（Unityと同じ挙動）
	if (!GUIWinMain::IsPlaying()) {
		// 編集操作の変更を記録する
		string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
		SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
		EditorSettingsManager::SetSaveFlag(true);
	}
}

void CommandManager::Undo() {
	if (GUIWinMain::IsPlaying()) { return; }
	if (Get().mUndoStack.empty())return;

	auto command = std::move(Get().mUndoStack.back());
	Get().mUndoStack.pop_back();

	command->Undo();
	Get().mRedoStack.push_back(std::move(command));

	// 編集操作の変更を記録する
	string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
	SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
	EditorSettingsManager::SetSaveFlag(true);
}

void CommandManager::Redo() {
	if (GUIWinMain::IsPlaying()) { return; }
	if (Get().mRedoStack.empty())return;

	auto command = std::move(Get().mRedoStack.back());
	Get().mRedoStack.pop_back();

	command->Execute();
	Get().mUndoStack.push_back(std::move(command));

	// 編集操作の変更を記録する
	string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
	SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
	EditorSettingsManager::SetSaveFlag(true);
}

void CommandManager::Shutdown() {
	Get().mUndoStack.clear();
	Get().mRedoStack.clear();
}

CommandManager& CommandManager::Get()
{
	static CommandManager instance;
	return instance;
}
