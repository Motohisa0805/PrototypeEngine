#pragma once
#include <string>
#include "ICommand.h"
#include "SelectionManager.h"

class CreateNewCanvasCommand : public ICommand
{
private:
	uint64_t		mTargetID;
	Canvas*			mTarget;
	// アクターが現在シーン側にいるかどうかのフラグ
	bool			mIsActiveInScene;
public:
	CreateNewCanvasCommand();
	~CreateNewCanvasCommand();

	void Execute()override;

	void NoHistoryExecute()override {}

	void Undo() override;

	void Redo()override;
};

