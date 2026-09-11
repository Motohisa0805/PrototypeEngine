#pragma once
#include "ICommand.h"
#include "SelectionManager.h"
#include <filesystem>
#include <string>

//サブメッシュをヒエラルキーにドロップしオブジェクトを生成するコマンド
class CreateActorFromSubMeshCommand : public ICommand
{
private:
    uint64_t              mCreateParentID;
    ActorObject*          mCreateParentActor;

    std::filesystem::path mAssetPath;
    string                mSubMeshName;
    string                mLocalID;

    ActorObject*          mParentActor;
    // アクターが現在シーン側にいるかどうかのフラグ
    bool mIsActiveInScene;

public:
    CreateActorFromSubMeshCommand(const std::filesystem::path& assetPath,const string& subMeshName,const string& localID,ActorObject* parentActor = nullptr);
    ~CreateActorFromSubMeshCommand();
    
    void ReleasePasteActor(ActorObject* actor);

    void Execute() override;

    void NoHistoryExecute() override {}

    void Undo() override;

    void Redo() override;
};
