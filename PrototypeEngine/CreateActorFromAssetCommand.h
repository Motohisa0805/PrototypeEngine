#pragma once
#include "ICommand.h"
#include "SelectionManager.h"
#include <filesystem>
#include <string>

class CreateActorFromAssetCommand : public ICommand
{
private:
    uint64_t              mTargetID;
    ActorObject*          mTarget;
    std::filesystem::path mAssetPath;

    ActorObject*          mParentActor;
    // アクターが現在シーン側にいるかどうかのフラグ
    bool mIsActiveInScene;

public:
    CreateActorFromAssetCommand(const std::filesystem::path& assetPath,
                                ActorObject* parentActor = nullptr);
    ~CreateActorFromAssetCommand();

    void Execute() override;

    void AddComponent(ActorObject* actor);

    void NoHistoryExecute() override {}

    void Undo() override;

    void Redo() override;
};
