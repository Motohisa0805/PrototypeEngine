#include "SceneEditorCamera.h"

SceneEditorCamera::SceneEditorCamera()
	: mAngularSpeed(0.0f)
	, mPitchSpeed(0.0f)
	, mMaxPitch(Math::Pi / 3.0f)
	, mPitch(0.0f)
	, mHorizontalMouseSpeed(500.0f)
	, mVertexMouseSpeed(500.0f)
{
}

void SceneEditorCamera::Update()
{
	//視点回転
	if (!Math::NearZero(mAngularSpeed))
	{
		Quaternion rot = mLocalRotation;
		mAngular = mAngularSpeed * Time::gDeltaTime;
		// Create quaternion for incremental rotation
		// (Rotate about up axis)
		Quaternion inc(Vector3::UnitY, mAngular);
		// Concatenate old and new quaternion
		rot = Quaternion::Concatenate(rot, inc);
		mLocalRotation = rot;
	}

	// カメラの位置はオーナーの位置です。
	Vector3 cameraPos = mLocalPosition;

	// ピッチ速度に基づいてピッチを更新する。
	mPitch += mPitchSpeed * Time::gDeltaTime;
	// クランプピッチを[-max, +max]に制限する。
	mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);
	// オーナーの右ベクトルを軸とするピッチ回転を表す四元数を作成します。
	Quaternion q(GetRight(), mPitch);

	// 所有者をピッチクォータニオンで前方に回転させる。
	Vector3 viewForward = Vector3::Transform(
		GetForward(), q);
	// 視線の前方100ユニットのターゲット位置。
	Vector3 target = cameraPos + viewForward * 100.0f;
	// ピッチクォータニオンを回転。
	Vector3 up = Vector3::Transform(Vector3::UnitY, q);

	// マトリックスを作成し、ビューとして設定します。
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	WindowRenderProperty::SetViewEye(cameraPos);
	WindowRenderProperty::SetViewTarget(target);
	WindowRenderProperty::SetViewUp(up);
	mViewMatrix = view;

	
	ComputeWorldTransform(NULL);
}

void SceneEditorCamera::ProcessInput(const struct InputState& keyState)
{
	// 右クリックが押されていない場合、何もしない
	if(!keyState.Mouse.GetButton(SDL_BUTTON_RIGHT))
	{
		return;
	}

	//SDLでマウスの移動数値を取得
	float x, y;
	SDL_GetRelativeMouseState(&x, &y);
	//マウスでの最大移動数値を設定

	// 最大速度での回転/秒
	const float maxAngularSpeed = Math::Pi * 8;
	float angularSpeed = 0.0f;
	if (x != 0)
	{
		// [-1.0, 1.0]に変換する
		angularSpeed = static_cast<float>(x) / mHorizontalMouseSpeed;
		// 回転/秒で掛ける
		angularSpeed *= maxAngularSpeed;
	}
	mAngularSpeed = angularSpeed;

	// 音高を計算する
	const float maxPitchSpeed = Math::Pi * 8;
	float pitchSpeed = 0.0f;
	if (y != 0)
	{
		// [-1.0, 1.0]に変換する
		pitchSpeed = static_cast<float>(y) / mVertexMouseSpeed;
		pitchSpeed *= maxPitchSpeed;
	}
	mPitchSpeed = pitchSpeed;
}