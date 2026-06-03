#pragma once
#include <string>
#include "ICommand.h"

//名前変更のコマンド
class RenameCommand : public ICommand
{
private:
	// 操作対象のアクター
	uint64_t		mTargetID;	
	// 変更前の名前（Undo用）
	string			mOldName;		
	// 変更後の名前（Execute用）
	string			mNewName;		
public:
	RenameCommand(Entity* actor, const string& newName);

	void Execute() override;

	void NoHistoryExecute()override {}

	void Undo() override;

	void Redo() override;
};

