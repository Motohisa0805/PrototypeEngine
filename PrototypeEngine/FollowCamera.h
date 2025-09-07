#pragma once
#include "BaseCamera.h"
#include "Actor.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//書籍部分を改造したファイル
//三人称視点カメラのクラス
//ターゲットの真後ろにカメラを配置し、
//マウスでカメラの向きを変更できるようにするクラス
class FollowCamera : public BaseCamera
{
private:
	//ターゲットの真後ろ限定の処理
	Vector3		ComputeCameraPos() const;
	//マウスで操作カメラの向きを更新できる処理
	Vector3		ComputeFreeCameraPos();

	// カメラの位置
	Vector3		mActualPos;
	// 実際のカメラの速度
	Vector3		mVelocity;
	// 水平追従距離
	float		mHorzDist;
	// 垂直フォロー距離
	float		mVertDist;
	// 目標との距離
	float		mTargetDist;
	// ばね定数（高いほど硬い）
	float		mSpringConstant;

	// ピッチの回転/秒速度。
	float		mPitchSpeed;

	float		mYaw = 0.0f;
	float		mPitch = 0.0f;

	//マウススピード
	float		mHorizontalMouseSpeed;
	float		mVertexMouseSpeed;
	//カメラを固定するかのフラグ
	bool		mFixed;
public:
				FollowCamera(ActorObject* owner);

	void		Update(float deltaTime) override;

	void		SnapToIdeal();

	void		SetHorzDist(float dist) { mHorzDist = dist; }
	void		SetVertDist(float dist) { mVertDist = dist; }
	void		SetTargetDist(float dist) { mTargetDist = dist; }
	void		SetSpringConstant(float spring) { mSpringConstant = spring; }

	float		GetYaw() { return mYaw; }
	float		GetPitch() { return mPitch; }
};

