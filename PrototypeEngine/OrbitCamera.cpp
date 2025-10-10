#include "OrbitCamera.h"

OrbitCamera::OrbitCamera(ActorObject* owner)
	:BaseCamera(owner)
	, mOffset(-4.0f, 0.0f, 0.0f)
	, mUp(Vector3::UnitY)
	, mPitchSpeed(0.0f)
	, mYawSpeed(0.0f)
{
	mName = "OrbitCamera";
}

void OrbitCamera::Update(float deltaTime)
{
	BaseCamera::Update(deltaTime);
	// ワールドアップに関するヨーのためのクォータニオンを作成する。
	Quaternion yaw(Vector3::UnitZ, mYawSpeed * deltaTime);
	// オフセットを変形してヨーで上に移動します。
	mOffset = Vector3::Transform(mOffset, yaw);
	mUp = Vector3::Transform(mUp, yaw);

	// これらのベクトルからカメラの前方/右を計算します。
	// Forward owner.position - (owner.position + offset)
	// = -offset
	Vector3 forward = -1.0f * mOffset;
	forward.Normalize();
	Vector3 right = Vector3::Cross(mUp, forward);
	right.Normalize();

	// カメラの右回りのピッチに対する四元数を作成する
	Quaternion pitch(right, mPitchSpeed * deltaTime);
	// ピッチでカメラのオフセットと上を変換する
	mOffset = Vector3::Transform(mOffset, pitch);
	mUp = Vector3::Transform(mUp, pitch);

	// 変換行列を計算する
	Vector3 target = mOwner->GetLocalPosition();
	Vector3 cameraPos = target + mOffset;
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, mUp);
	SetViewMatrix(view);
}