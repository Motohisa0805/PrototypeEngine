#pragma once
#include "Component.h"
#include "Math.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//カメラの基底クラス
class BaseCamera : public Component
{
public:
protected:
	void SetViewMatrix(const Matrix4& view);

	Vector3 mCameraForward;
	Vector3 mCameraRight;
	Vector3 mCameraUp;

	Matrix4 mCameraYawRot;

	bool	mIsMain; // メインカメラかどうか

	Matrix4	mView;
public:
	BaseCamera(class ActorObject* owner, int updateOrder = 200);
	~BaseCamera();

	Vector3 GetCameraForward() { return mCameraForward; }
	Vector3 GetCameraRight() { return mCameraRight; }
	Vector3 GetCameraUp() { return mCameraUp; }
	Matrix4 GetCameraYawRot() { return mCameraYawRot; }
	bool IsMain() const { return mIsMain; }
	void SetIsMain(bool isMain) { mIsMain = isMain; }
	Matrix4 GetViewMatrix() const { return mView; }
};