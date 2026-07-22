#pragma once
#include "ICommand.h"
#include "SelectionManager.h"
#include <filesystem>
#include <string>

//メッシュファイルをヒエラルキーにドロップしオブジェクトを生成するコマンド
//現在はサブメッシュだけに対応
class CreateActorFromAssetCommand : public ICommand
{
private:
    uint64_t              mTargetID;
    ActorObject*          mTarget;
    std::filesystem::path mAssetPath;
    string                mLocalID;

    ActorObject*          mParentActor;
    // アクターが現在シーン側にいるかどうかのフラグ
    bool mIsActiveInScene;

public:
    CreateActorFromAssetCommand(const std::filesystem::path& assetPath,const string& localID,ActorObject* parentActor = nullptr);
    ~CreateActorFromAssetCommand();

    void Execute() override;

    void AddComponent(ActorObject* actor);

    void NoHistoryExecute() override {}

    void Undo() override;

    void Redo() override;
};
