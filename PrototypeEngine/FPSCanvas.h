#pragma once
#include "Canvas.h"
#include "PhysWorld.h"
#include "TargetComponent.h"
#include "Actor.h"
#include "TargetComponentSystem.h"

//�Q�[�����UI���܂Ƃ߂ď�������N���X
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



	// ���[�_�[�ɑ΂���u���b�v��2D�I�t�Z�b�g
	vector<Vector2>							mBlips;
	// ���[�_�[�͈̔͂Ɣ��a�𒲐�����
	float									mRadarRange;
	float									mRadarRadius;
	// �N���X�w�A���G��_���Ă��邩�ǂ���
	bool									mTargetEnemy;

	float									mCrosshairAngle;

	TargetComponentSystem*					mTargetComponentSystem;
public:
	// (���Ɉ��������͌��ɑΉ����܂�)
											FPSCanvas();
											~FPSCanvas();

	void									Update(float deltaTime) override;

	void									ProcessInput(const struct InputState& keys)override;

	void									AddTargetComponent(class TargetComponent* tc);
	void									RemoveTargetComponent(class TargetComponent* tc);


	Image*									GetHelthBar() { return mHelthBar; }
};