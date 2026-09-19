#include "SelectionManager.h"
#include "AnimatorNodeEditorPanel.h"

Entity* SelectionManager::mSelectedActor = nullptr;

filesystem::path SelectionManager::mSelectedFilePath = "Assets";

string SelectionManager::mSelectedStateName = "";

void SelectionManager::SetSelectedActor(Entity* actor) 
{
    mSelectedActor    = actor;
    mSelectedFilePath = "Assets";
    mSelectedStateName.clear();
    if (actor)
    {
        auto* animator    = actor->GetComponent<Animator>();
        if (animator)
        {
            AnimatorNodeEditorPanel::SetSelectAnimator(animator);
        }
    }
}
