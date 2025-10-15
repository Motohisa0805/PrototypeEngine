#pragma once
#include "Transform.h"

#define MAX_YAW_SPEED 50


//GUI�̃G�f�B�^�[�V�[���̃J�����̏������s���N���X
class SceneEditorCamera : public Transform
{
private:

	bool	mIsViewDirty;

	Matrix4	mViewMatrix;

	float	mYawSpeed;
	// �s�b�`�̉�]/�b���x�B
	float	mPitchSpeed;
	// �O������̍ő�s�b�`�΍��B
	float	mMaxPitch;
	// ���݂̃s�b�`�B
	float	mPitch;
	//�}�E�X�X�s�[�h
	float	mMouseSensitivityX;
	float	mMouseSensitivityY;
	//�O��ړ��X�s�[�h
	float	mForwardSpeed;
	//���E�ړ��X�s�[�h
	float	mStrafeSpeed;
	//�ő�X�s�[�h
	float   mMaxSpeed;
	//�ŏ��X�s�[�h
	float   mMinSpeed;
	//�X�s�[�h
	float	mSpeed;

public:
	SceneEditorCamera();


	void	Update();

	//��l�̎��_�̃J�����������s���s�b�`���x���擾����֐�
	void	ProcessInput(const struct InputState& keyState);
	float	GetAngularSpeed() const { return mYawSpeed; }
	float	GetPitch() const { return mPitch; }
	float	GetPitchSpeed() const { return mPitchSpeed; }
	float	GetMaxPitch() const { return mMaxPitch; }

	float	GetHorizontalMouseSpeed()const { return mMouseSensitivityX; }
	float	GetVertexMouseSpeed()const { return mMouseSensitivityY; }

	void	SetAngularSpeed(float speed) { mYawSpeed = speed; }
	void	SetPitchSpeed(float speed) { mPitchSpeed = speed; }
	void	SetMaxPitch(float pitch) { mMaxPitch = pitch; }

	void	SetHorizontalMouseSpeed(float speed) { mMouseSensitivityX = speed; }
	void	SetVertextMouseSpeed(float speed) { mMouseSensitivityY = speed; }

	Matrix4 GetViewMatrix() { return mViewMatrix; }
};

