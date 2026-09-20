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

    //遷移フラグ管理UI表示フラグ
    bool                                mIsTransitionUIFrag;

public:
    AnimatorNodeEditorPanel(Renderer* renderer);
    // GUIの初期化
    void                Initialize(float width, float height,ImTextureRef ref = nullptr) override;
    void                ParameterDraw();
    // GUIの描画
    void                Draw(float width, float height) override;

    void                DeletedNode(int index,AnimatorControllerParameters& controllerData);

    static Animator*    GetSelectAnimator() { return mSelectAnimator; }
    static void         SetSelectAnimator(Animator* animator) {mSelectAnimator = animator;}
};
