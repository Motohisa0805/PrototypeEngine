#pragma once
#include <string>
#include "ICommand.h"
#include "SelectionManager.h"
#include "EditorClipboard.h"

//貼り付けコマンドクラス
class PasteActorCommand : public ICommand
{
private:
	// ペーストによって生成されたアクター
	ActorObject*	mTarget; 
	bool			mIsActiveInScene;
public:
	PasteActorCommand();
	~PasteActorCommand();

	void Execute() override;

	void Undo() override;

	void Redo()override;
};

