#pragma once
#include <string>
#include "ICommand.h"
#include "SelectionManager.h"

//ヒエラルキー内の親子関係を含めた移動コマンド
class ReparentAndReorderCommand : public ICommand
{
private:
	ActorObject*	mTarget;

	ActorObject*	mOldParent;
	// 移動前、旧親（またはルート）のリスト内でのインデックス
	size_t			mFromIndex;

	ActorObject*	mNewParent;
	// 移動後、新親（またはルート）のリスト内でのインデックス
	size_t			mToIndex;
public:
	ReparentAndReorderCommand(ActorObject* target, ActorObject* newParent, size_t toIndex);

	void Execute()override;

	void Undo() override;

	void Redo()override;
};

