#include "RenameCommand.h"

RenameCommand::RenameCommand(Entity* actor, const string& newName)
    : mTargetID(actor->GetID()), mNewName(newName)
{
    // 変更前の名前を保存
    mOldName = actor->GetName();
}

void RenameCommand::Execute()
{
    // アクターの名前を変更
    if (mTargetID != 0)
    {
        ActorObject*   actor   = SceneManager::GetCurrentRunScene()
                                     ->GetActorManager()
                                     ->FindActorByID(mTargetID);
        UIActorObject* uiactor = SceneManager::GetCurrentRunScene()
                                     ->GetUIActorManager()
                                     ->FindActorByID(mTargetID);
        if (actor)
        {
            actor->SetName(mNewName);
        }
        else if (uiactor)
        {
            uiactor->SetName(mNewName);
        }
    }
}

void RenameCommand::Undo()
{
    // アクターの名前を元に戻す
    if (mTargetID != 0)
    {
        ActorObject*   actor   = SceneManager::GetCurrentRunScene()
                                     ->GetActorManager()
                                     ->FindActorByID(mTargetID);
        UIActorObject* uiactor = SceneManager::GetCurrentRunScene()
                                     ->GetUIActorManager()
                                     ->FindActorByID(mTargetID);
        if (actor)
        {
            actor->SetName(mOldName);
        }
        else if (uiactor)
        {
            uiactor->SetName(mOldName);
        }
    }
}

void RenameCommand::Redo() { Execute(); }
