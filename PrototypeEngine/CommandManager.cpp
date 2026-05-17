#include "CommandManager.h"

void CommandManager::Execute(std::unique_ptr<ICommand> command)
{
	command->Execute();

	// 新しい操作が行われたらRedoスタックはクリアする（Unityと同じ挙動）
	Get().mRedoStack.clear();
	Get().mUndoStack.push_back(std::move(command));

	if (Get().mUndoStack.size() > 50) {
		Get().mUndoStack.erase(Get().mUndoStack.begin()); // 一番古いコマンドを削除
	}
}

void CommandManager::Undo() {
	if (Get().mUndoStack.empty())return;

	auto command = std::move(Get().mUndoStack.back());
	Get().mUndoStack.pop_back();

	command->Undo();
	Get().mRedoStack.push_back(std::move(command));
}

void CommandManager::Redo() {
	if (Get().mRedoStack.empty())return;

	auto command = std::move(Get().mRedoStack.back());
	Get().mRedoStack.pop_back();

	command->Execute();
	Get().mUndoStack.push_back(std::move(command));
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
