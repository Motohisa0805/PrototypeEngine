#pragma once
#include <string>
#include "ICommand.h"
#include "SelectionManager.h"
#include "EditorClipboard.h"

//複製コマンドクラス
class DuplicateCommand : public ICommand
{
private:
	// 複製によって生成されたアクターID
	uint64_t		mTargetID;
	Entity*			mTarget;
	bool			mIsActiveInScene;
public:
	DuplicateCommand();
	~DuplicateCommand();

	void Execute() override;

	void NoHistoryExecute()override{}

	void Undo() override;

	void Redo()override;
};

