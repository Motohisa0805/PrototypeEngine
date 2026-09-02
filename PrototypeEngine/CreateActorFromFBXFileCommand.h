#pragma once
#include "ICommand.h"
#include "SelectionManager.h"
#include <filesystem>
#include <string>

// FBXファイルをヒエラルキーにドロップしオブジェクトを生成するコマンド
class CreateActorFromFBXFileCommand : public ICommand
{
private:
    // FBXファイルから生成されたアクターのIDを保持するための変数
	vector<uint64_t>      mTargetIDs;
	vector<ActorObject*>  mTargets;
	std::filesystem::path mAssetPath;
	ActorObject*          mParentActor;
	// アクターが現在シーン側にいるかどうかのフラグ
    bool                  mIsActiveInScene;

public:
    CreateActorFromFBXFileCommand(const std::filesystem::path& assetPath, ActorObject* parentActor = nullptr);
    ~CreateActorFromFBXFileCommand();

	void Execute() override;

    void NoHistoryExecute() override {}

    void Undo() override;

    void Redo() override;
};
