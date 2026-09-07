#pragma once
#include <string>
#include "ICommand.h"
#include "SelectionManager.h"

//オブジェクトの削除コマンド
class DeleteCommand : public ICommand
{
private:
	// 削除対象のアクター配列
    vector<CommandTargetData>	mTargets;
	//現在シーンに存在するかどうかのフラグ
	bool						mIsActiveInScene;

	void						CollectTargets(Entity* actor);

public:
	DeleteCommand(Entity* actor);
	~DeleteCommand();

	void Execute() override;

	void NoHistoryExecute()override {}

	void Undo() override;

	void Redo() override;
};

