#include "CreateActorFromFBXFileCommand.h"

CreateActorFromFBXFileCommand::CreateActorFromFBXFileCommand(
    const std::filesystem::path& assetPath, ActorObject* parentActor)
    : mAssetPath(assetPath)
    , mParentActor(parentActor)
    , mIsActiveInScene(false)
    , mTargetIDs()
    , mTargets()
{
}

CreateActorFromFBXFileCommand::~CreateActorFromFBXFileCommand() 
{
    if (!mIsActiveInScene)
    {
        for (ActorObject* target : mTargets)
        {
            delete target;
        }
    }
}

void CreateActorFromFBXFileCommand::Execute() {}

void CreateActorFromFBXFileCommand::Undo() {}

void CreateActorFromFBXFileCommand::Redo() { Execute(); }
