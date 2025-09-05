#pragma once
#include "Transform.h"


//GUIのエディターシーンのカメラの処理を行うクラス
class SceneEditorCamera : public Transform
{
private:
	Matrix4	mViewMatrix;

	float	mAngularSpeed;
	// ピッチの回転/秒速度。
	float	mPitchSpeed;
	// 前方からの最大ピッチ偏差。
	float	mMaxPitch;

	float	mAngular;
	// 現在のピッチ。
	float	mPitch;

	//マウススピード
	float	mHorizontalMouseSpeed;
	float	mVertexMouseSpeed;

	float   mLastMouseX;
	float   mLastMouseY;

	float	mForwardSpeed;

	float	mStrafeSpeed;

public:
	SceneEditorCamera();


	void	Update();

	//一人称視点のカメラ処理を行いピッチ速度を取得する関数
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

