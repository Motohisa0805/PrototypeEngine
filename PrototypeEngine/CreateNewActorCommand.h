#pragma once
#include <string>
#include "ICommand.h"
#include "SelectionManager.h"

//新規アクターを生成するコマンド
class CreateNewActorCommand : public ICommand
{
private:
	uint64_t		mTargetID;
	ActorObject*	mTarget;
	// アクターが現在シーン側にいるかどうかのフラグ
	bool			mIsActiveInScene;        
public:
	CreateNewActorCommand();
	~CreateNewActorCommand();

	void Execute()override;

	void Undo() override;

	void Redo()override;
};

