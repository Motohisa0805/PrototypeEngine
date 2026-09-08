#pragma once
#include "ICommand.h"
#include "SelectionManager.h"
#include <filesystem>
#include <string>

// FBXファイルをヒエラルキーにドロップしオブジェクトを生成するコマンド
class CreateActorFromFBXFileCommand : public ICommand
{
private:
    uint64_t              mCreateParentID;
    ActorObject*          mCreateParentActor;


	std::filesystem::path mAssetPath;
	ActorObject*          mParentActor;
	// アクターが現在シーン側にいるかどうかのフラグ
    bool                  mIsActiveInScene;

public:
    CreateActorFromFBXFileCommand(const std::filesystem::path& assetPath, ActorObject* parentActor = nullptr);
    ~CreateActorFromFBXFileCommand();

    void ReleasePasteActor(ActorObject* actor);

	void Execute() override;

    void NoHistoryExecute() override {}

    void Undo() override;

    void Redo() override;
};
