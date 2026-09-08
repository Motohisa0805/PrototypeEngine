#pragma once
#include <string>
#include "ICommand.h"
#include "SelectionManager.h"
#include "EditorClipboard.h"

//貼り付けコマンドクラス
class PasteActorCommand : public ICommand
{
private:
    // 削除対象のアクター配列
    vector<CommandTargetData> mTargets;
    // 現在シーンに存在するかどうかのフラグ
	bool			mIsActiveInScene;

	// ペーストによって生成されたアクター
	uint64_t		mTargetID; 
	Entity*			mTarget; 

	void CollectTargets(Entity* actor);
public:
	PasteActorCommand();
	~PasteActorCommand();

	void ReleasePasteActor(Entity* actor);

	void Execute() override;
	
	void NoHistoryExecute()override;

	void Undo() override;

	void Redo()override;
};

