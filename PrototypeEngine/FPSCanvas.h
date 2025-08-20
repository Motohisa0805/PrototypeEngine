#pragma once
#include "Canvas.h"
#include "PhysWorld.h"
#include "TargetComponent.h"
#include "Actor.h"
#include "TargetComponentSystem.h"

//ゲーム上のUIをまとめて処理するクラス
class FPSCanvas : public Canvas
{
protected:
	void									UpdateCrosshair(float deltaTime);
	void									UpdateBlipTextures();
	void									UpdateRadar(float deltaTime);

	Image*									mRadar;

	Image*									mCrosshair;
	Image*									mCrosshairEnemy;

	Image*									mBlipTex;

	vector<class Image*>					mBlipTexs;

	Image*									mRadarArrow;

	Image*									mHelthBarFrame;

	Image*									mHelthBar;

	Text*									mSceneNameText;

	Image*									mSceneNameFrame;

	Text*									mPoseButtonText;

	Image*									mPoseButtonFrame;

	Image*									mPoseButton;



	// レーダーに対するブリップの2Dオフセット
	vector<Vector2>							mBlips;
	// レーダーの範囲と半径を調整する
	float									mRadarRange;
	float									mRadarRadius;
	// クロスヘアが敵を狙っているかどうか
	bool									mTargetEnemy;

	float									mCrosshairAngle;

	TargetComponentSystem*					mTargetComponentSystem;
public:
	// (下に引く順序は後ろに対応します)
											FPSCanvas();
											~FPSCanvas();

	void									Update(float deltaTime) override;

	void									ProcessInput(const struct InputState& keys)override;

	void									AddTargetComponent(class TargetComponent* tc);
	void									RemoveTargetComponent(class TargetComponent* tc);


	Image*									GetHelthBar() { return mHelthBar; }
};