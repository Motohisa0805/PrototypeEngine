#include "SelectionManager.h"
#include "AnimatorNodeEditorPanel.h"

Entity* SelectionManager::mSelectedActor = nullptr;

filesystem::path SelectionManager::mSelectedFilePath = "Assets";

void SelectionManager::SetSelectedActor(Entity* actor) 
{
    mSelectedActor    = actor;
    mSelectedFilePath = "Assets";
    if (actor)
    {
        auto* animator    = actor->GetComponent<Animator>();
        if (animator)
        {
            AnimatorNodeEditorPanel::SetSelectAnimator(animator);
        }
    }
}
