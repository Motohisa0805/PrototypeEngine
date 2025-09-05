#pragma once
#include "Transform.h"


//GUI�̃G�f�B�^�[�V�[���̃J�����̏������s���N���X
class SceneEditorCamera : public Transform
{
private:
	Matrix4	mViewMatrix;

	float	mAngularSpeed;
	// �s�b�`�̉�]/�b���x�B
	float	mPitchSpeed;
	// �O������̍ő�s�b�`�΍��B
	float	mMaxPitch;

	float	mAngular;
	// ���݂̃s�b�`�B
	float	mPitch;

	//�}�E�X�X�s�[�h
	float	mHorizontalMouseSpeed;
	float	mVertexMouseSpeed;

	float   mLastMouseX;
	float   mLastMouseY;

	float	mForwardSpeed;

	float	mStrafeSpeed;

public:
	SceneEditorCamera();


	void	Update();

	//��l�̎��_�̃J�����������s���s�b�`���x���擾����֐�
	void	ProcessInput(const struct InputState& keyState);

	float	GetAngular() const { return mAngular; }
	float	GetAngularSpeed() const { return mAngularSpeed; }
	float	GetPitch() const { return mPitch; }
	float	GetPitchSpeed() const { return mPitchSpeed; }
	float	GetMaxPitch() const { return mMaxPitch; }

	float	GetHorizontalMouseSpeed()const { return mHorizontalMouseSpeed; }
	float	GetVertexMouseSpeed()const { return mVertexMouseSpeed; }

	void	SetAngularSpeed(float speed) { mAngularSpeed = speed; }
	void	SetPitchSpeed(float speed) { mPitchSpeed = speed; }
	void	SetMaxPitch(float pitch) { mMaxPitch = pitch; }

	void	SetHorizontalMouseSpeed(float speed) { mHorizontalMouseSpeed = speed; }
	void	SetVertextMouseSpeed(float speed) { mVertexMouseSpeed = speed; }

	Matrix4 GetViewMatrix() { return mViewMatrix; }
};

