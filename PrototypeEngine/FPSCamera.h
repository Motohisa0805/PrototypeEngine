#pragma once
#include "BaseCamera.h"
#include "Actor.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//書籍部分を一部改造したファイル
//一人称視点カメラのクラス
class FPSCamera : public BaseCamera
{
private:
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
public:
			FPSCamera(class ActorObject* owner);

	void	Update(float deltaTime) override;

	//一人称視点のカメラ処理を行いピッチ速度を取得する関数
	void	CameraInputUpdate();

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
};