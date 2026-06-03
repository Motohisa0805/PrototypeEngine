#pragma once
#include <string>
#include "ICommand.h"
#include "SelectionManager.h"

//ヒエラルキー内の親子関係を含めた移動コマンド
class ReparentAndReorderCommand : public ICommand
{
private:
    uint64_t    mTargetID;      // 操作対象アクターのID
    uint64_t    mOldParentID;   // 移動前の親アクターのID（ルートなら0）
    uint64_t    mNewParentID;   // 移動後の親アクターのID（ルートなら0）

    size_t      mFromIndex;       // 移動前のリスト内でのインデックス
    size_t      mToIndex;         // 移動後のリスト内でのインデックス
public:
	ReparentAndReorderCommand(Entity* target, Entity* newParent, size_t toIndex);

	void Execute()override;

    void NoHistoryExecute()override {}

	void Undo() override;

	void Redo()override;

    // IDから対象となる「変更可能なアクターリスト」を取得する内部ヘルパー関数
    vector<ActorObject*>& GetActorListMutable(uint64_t parentID);
    vector<UIActorObject*>& GetUIActorListMutable(uint64_t parentID);
};

