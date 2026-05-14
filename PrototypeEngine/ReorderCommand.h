#pragma once
#include <string>
#include "ICommand.h"
#include "SelectionManager.h"

//ヒエラルキー内のオブジェクト移動コマンド
class ReorderCommand : public ICommand
{
private:
	vector<ActorObject*>&	mActors;
	size_t					mFromIndex;
	size_t					mToIndex;
public:
	ReorderCommand(vector<ActorObject*>& actors, size_t fromIndex, size_t toIndex);
	~ReorderCommand();

	void Execute() override;

	void Undo() override;

	void Redo() override;
};

