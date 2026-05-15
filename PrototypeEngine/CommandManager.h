#pragma once
#include "Typedefs.h"
#include <memory>
#include "ICommand.h"
#include "RenameCommand.h"
#include "DeleteCommand.h"
#include "ReorderCommand.h"
#include "ReparentAndReorderCommand.h"
#include "CreateNewActorCommand.h"
#include "PasteActorCommand.h"

//Undo/Redoの司令塔
class CommandManager
{
private:
	vector<std::unique_ptr<ICommand>> mUndoStack;
	vector<std::unique_ptr<ICommand>> mRedoStack;
public:
	//新しい操作を実行し、Undoスタッグに追加
	static void Execute(std::unique_ptr<ICommand> command) {
		command->Execute();

		// 新しい操作が行われたらRedoスタックはクリアする（Unityと同じ挙動）
		Get().mRedoStack.clear();
		Get().mUndoStack.push_back(std::move(command));
	}

	static void Undo() {
		if (Get().mUndoStack.empty())return;

		auto command = std::move(Get().mUndoStack.back());
		Get().mUndoStack.pop_back();

		command->Undo();
		Get().mRedoStack.push_back(std::move(command));

		if (Get().mUndoStack.size() > 50) {
			Get().mUndoStack.erase(Get().mUndoStack.begin()); // 一番古いコマンドを削除
		}
	}

	static void Redo() {
		if (Get().mRedoStack.empty())return;

		auto command = std::move(Get().mRedoStack.back());
		Get().mRedoStack.pop_back();

		command->Execute();
		Get().mUndoStack.push_back(std::move(command));
	}

	static void Shutdown() {
		Get().mUndoStack.clear();
		Get().mRedoStack.clear();
	}
private:
	static CommandManager& Get() {
		static CommandManager instance;
		return instance;
	}
};

