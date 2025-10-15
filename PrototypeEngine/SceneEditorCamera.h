#pragma once
#include "Transform.h"

#define MAX_YAW_SPEED 50


//GUIのエディターシーンのカメラの処理を行うクラス
class SceneEditorCamera : public Transform
{
private:

	bool	mIsViewDirty;

	Matrix4	mViewMatrix;

	float	mYawSpeed;
	// ピッチの回転/秒速度。
	float	mPitchSpeed;
	// 前方からの最大ピッチ偏差。
	float	mMaxPitch;
	// 現在のピッチ。
	float	mPitch;
	//マウススピード
	float	mMouseSensitivityX;
	float	mMouseSensitivityY;
	//前後移動スピード
	float	mForwardSpeed;
	//左右移動スピード
	float	mStrafeSpeed;
	//最大スピード
	float   mMaxSpeed;
	//最少スピード
	float   mMinSpeed;
	//スピード
	float	mSpeed;

public:
	SceneEditorCamera();


	void	Update();

	//一人称視点のカメラ処理を行いピッチ速度を取得する関数
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

