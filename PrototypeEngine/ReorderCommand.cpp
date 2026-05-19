#include "ReorderCommand.h"

ReorderCommand::ReorderCommand(vector<ActorObject*>& actors, size_t fromIndex, size_t toIndex)
	:mActors(actors)
	,mFromIndex(fromIndex)
	,mToIndex(toIndex)
{
}

ReorderCommand::~ReorderCommand()
{
}

void ReorderCommand::Execute()
{
	if (mFromIndex < mActors.size() && mToIndex < mActors.size()) {
		ImGuiHelper::ReorderVector(mActors, mFromIndex, mToIndex);
	}
}

void ReorderCommand::Undo()
{
	if (mFromIndex < mToIndex) {
		// 後ろに移動していた場合：移動後の位置(mToIndex - 1)から元の位置(mFromIndex)へ挿入
		ImGuiHelper::ReorderVector(mActors, mToIndex - 1, mFromIndex);
	}
	else {
		// 前に移動していた場合：移動後の位置(mToIndex)から元の位置(mFromIndex + 1)へ挿入
		ImGuiHelper::ReorderVector(mActors, mToIndex, mFromIndex + 1);
	}
}

void ReorderCommand::Redo()
{
	Execute();
}
