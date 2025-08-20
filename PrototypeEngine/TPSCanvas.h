#pragma once
#include "Canvas.h"
#include "PhysWorld.h"
#include "TargetComponent.h"
#include "Actor.h"

class TPSCanvas : public Canvas
{
protected:

	Image*	mSceneNameFrame;

	Text*	mPoseButtonText;

	Image*	mPoseButtonFrame;

	Image*	mPoseButton;

	Text*	mSceneLoadButtonText;

	//***アニメーション再生ボタンテキスト***
	Text*	mSceneNameText;

	Image*	mPoseFrame;

	Text*	mTPoseButtonText;

	Text*	mIdlePoseButtonText;

	Text*	mRunPoseButtonText;

	Text*	mJumpPoseButtonText;

	Text*	mCapoeiraPoseButtonText;

	Image*	mHelthBarFrame;

	Image*	mHelthBar;
public:
	// (下に引く順序は後ろに対応します)
	TPSCanvas();

	void									Update(float deltaTime) override;

	void									ProcessInput(const InputState& keys) override;


	Image*									GetHelthBar() { return mHelthBar; }
};

