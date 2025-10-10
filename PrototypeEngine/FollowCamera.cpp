#include "FollowCamera.h"

FollowCamera::FollowCamera(ActorObject* owner)
	:BaseCamera(owner)
	, mHorzDist(3.5f)
	, mVertDist(2.4f)
	, mTargetDist(2.5f)
	, mSpringConstant(12.8f)
	, mHorizontalMouseSpeed(200.0f)
	, mVertexMouseSpeed(200.0f)
{
	mName = "FollowCamera";
	mFixed = false;
}

void FollowCamera::Update(float deltaTime)
{
	BaseCamera::Update(deltaTime);
	if (!mFixed)
	{
		// 理想的な位置を計算する
		Vector3 idealPos = ComputeFreeCameraPos();
		// カメラ位置を即座に理想位置に設定
		mActualPos = idealPos;
		// ターゲットは所有者アクター
		Vector3 target = mOwner->GetPosition();
		target.y += 1.5f;
		// カメラのビュー行列を作成
		Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitY);
		WindowRenderProperty::SetViewEye(mActualPos);
		WindowRenderProperty::SetViewTarget(target);
		WindowRenderProperty::SetViewUp(Vector3::UnitY);
		SetViewMatrix(view);
	}
	else
	{
		// ばね定数から減衰を計算する
		float dampening = 2.0f * Math::Sqrt(mSpringConstant);
		// 理想的な位置を計算する
		Vector3 idealPos = ComputeCameraPos();
		// 実際と理想の違いを計算する
		Vector3 diff = mActualPos - idealPos;
		// ばねの加速度を計算する
		Vector3 acel = -mSpringConstant * diff -
			dampening * mVelocity;
		// 更新速度
		mVelocity += acel * deltaTime;
		// 実際のカメラ位置を更新する
		mActualPos += mVelocity * deltaTime;
		// ターゲットは所有者アクターの前方にあるターゲット距離
		Vector3 target = mOwner->GetLocalPosition() +
			mOwner->GetForward() * mTargetDist;
		// 理想ではなく、ここに実際の位置を使用してください。
		Matrix4 view = Matrix4::CreateLookAt(mActualPos, target,
			Vector3::UnitY);
		WindowRenderProperty::SetViewEye(mActualPos);
		WindowRenderProperty::SetViewTarget(target);
		WindowRenderProperty::SetViewUp(Vector3::UnitY);
		SetViewMatrix(view);
	}
}

void FollowCamera::SnapToIdeal()
{
	// 実際の位置を理想に設定する
	mActualPos = ComputeCameraPos();
	// ゼロ速度
	mVelocity = Vector3::Zero;
	// ターゲットを計算して表示
	Vector3 target = mOwner->GetLocalPosition() +
		mOwner->GetForward() * mTargetDist;
	// 理想ではなく、ここに実際の位置を使用してください。
	Matrix4 view = Matrix4::CreateLookAt(mActualPos, target,
		Vector3::UnitZ);
	SetViewMatrix(view);
}

Vector3 FollowCamera::ComputeCameraPos() const
{
	// Actorの後ろと上にカメラの位置を設定する
	Vector3 cameraPos = mOwner->GetLocalPosition();
	cameraPos -= mOwner->GetForward() * mHorzDist;
	cameraPos += Vector3::UnitZ * mVertDist;
	return cameraPos;
}

Vector3 FollowCamera::ComputeFreeCameraPos()
{
	Vector3 target = mOwner->GetPosition();

	// SDLでマウスの相対移動を取得
	float dx, dy;
	SDL_GetRelativeMouseState(&dx, &dy);

	const float maxAngularSpeed = Math::Pi * 8.0f;

	// ヨー（左右回転）：Y軸を中心に回転
	mYaw += (static_cast<float>(dx) / mHorizontalMouseSpeed) * maxAngularSpeed * Time::gDeltaTime;

	// ピッチ（上下回転）：X軸を中心に回転
	mPitch += (static_cast<float>(dy) / mVertexMouseSpeed) * maxAngularSpeed * Time::gDeltaTime;

	// ピッチの範囲制限（真上・真下を防ぐ）
	mPitch = Math::Clamp(mPitch, -Math::Pi / 2.0f + 0.1f, Math::Pi / 2.0f - 0.1f);

	// 球面座標系からオフセット計算
	Vector3 offset;
	offset.x = mHorzDist * cosf(mPitch) * sinf(mYaw);
	offset.y = mHorzDist * sinf(mPitch);
	offset.z = mHorzDist * cosf(mPitch) * cosf(mYaw);

	// カメラ位置 = プレイヤー位置 + オフセット
	Vector3 cameraPos = target + offset;

	// カメラの前方ベクトル（プレイヤー方向を向く）
	mCameraForward = Vector3::Normalize(target - cameraPos);

	// ワールドの上方向
	Vector3 worldUp = Vector3::UnitY;

	// カメラの右方向 = up × forward
	mCameraRight = Vector3::Normalize(Vector3::Cross(worldUp, mCameraForward));

	// カメラの上方向 = forward × right
	mCameraUp = Vector3::Cross(mCameraForward, mCameraRight);

	mCameraYawRot = Matrix4::CreateRotationY(mYaw);

	return cameraPos;
}
