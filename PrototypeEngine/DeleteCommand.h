#pragma once
#include <string>
#include "ICommand.h"
#include "SelectionManager.h"

//オブジェクトの削除コマンド
class DeleteCommand : public ICommand
{
private:
	// 削除対象のアクター
	uint64_t		mTargetID;
	ActorObject*	mTarget;
	//現在シーンに存在するかどうかのフラグ
	bool			mIsActiveInScene;
public:
	DeleteCommand(ActorObject* actor);
	~DeleteCommand();

	void Execute() override;

	void Undo() override;

	void Redo() override;
};

