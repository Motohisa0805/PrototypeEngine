#pragma once
#include <string>
#include "ICommand.h"
#include "SelectionManager.h"

//新規アクターを生成するコマンド(UIアクター版)
class CreateNewUIActorCommand : public ICommand
{
private:
	uint64_t		mTargetID;
	UIActorObject*  mTarget;
	// アクターが現在シーン側にいるかどうかのフラグ
	bool			mIsActiveInScene;
public:
	CreateNewUIActorCommand();
	~CreateNewUIActorCommand();

	void Execute()override;

	void NoHistoryExecute()override {}

	void Undo() override;

	void Redo()override;
};

