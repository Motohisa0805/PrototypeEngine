#include "SelectionManager.h"
#include "AnimatorNodeEditorPanel.h"

Entity* SelectionManager::mSelectedActor = nullptr;

filesystem::path SelectionManager::mSelectedFilePath = "Assets";

string SelectionManager::mSelectedStateName = "";

string SelectionManager::mSelectedTransitionName = "";

void SelectionManager::SetSelectedActor(Entity* actor) 
{
    mSelectedActor    = actor;
    mSelectedFilePath = "Assets";
    mSelectedStateName.clear();
    mSelectedTransitionName.clear();
    if (actor)
    {
        auto* animator    = actor->GetComponent<Animator>();
        if (animator)
        {
            AnimatorNodeEditorPanel::SetSelectAnimator(animator);
        }
    }
}
