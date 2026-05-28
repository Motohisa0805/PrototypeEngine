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
	uint64_t		mTargetID; 
	Entity*			mTarget; 
	bool			mIsActiveInScene;
public:
	PasteActorCommand();
	~PasteActorCommand();

	void Execute() override;

	void Undo() override;

	void Redo()override;
};

