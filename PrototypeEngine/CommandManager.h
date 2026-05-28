#pragma once
#include "Typedefs.h"
#include <memory>
#include "ICommand.h"
#include "RenameCommand.h"
#include "DeleteCommand.h"
#include "ReorderCommand.h"
#include "ReparentAndReorderCommand.h"
#include "CreateNewActorCommand.h"
#include "CreateNewUIActorCommand.h"
#include "CreateNewCanvasCommand.h"
#include "PasteActorCommand.h"

//Undo/Redoの司令塔
class CommandManager
{
private:
	vector<std::unique_ptr<ICommand>> mUndoStack;
	vector<std::unique_ptr<ICommand>> mRedoStack;
public:
	//新しい操作を実行し、Undoスタッグに追加
	static void Execute(std::unique_ptr<ICommand> command);

	static void Undo();

	static void Redo();

	static void Shutdown();
private:
	static CommandManager& Get();
};

