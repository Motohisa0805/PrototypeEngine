#pragma once
#include "EditorWindow.h"
#include "AnimatorControllerParameters.h"
#include "Animator.h"

class AnimatorNodeEditorPanel : public EditorWindow
{
private:
    static Animator*                    mSelectAnimator;
    static Animator*                    mPreviousAnimator;

    bool                                mNeedSetNodePositions;

public:
    AnimatorNodeEditorPanel(Renderer* renderer);
    // GUIÇÃèâä˙âª
    void             Initialize(float width, float height,ImTextureRef ref = nullptr) override;
    // GUIÇÃï`âÊ
    void             Draw(float width, float height) override;

    void             DeletedNode(int index,AnimatorControllerParameters& controllerData);

    static Animator* GetSelectAnimator() { return mSelectAnimator; }
    static void      SetSelectAnimator(Animator* animator) {mSelectAnimator = animator;}
};
