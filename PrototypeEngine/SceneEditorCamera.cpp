#include "SceneEditorCamera.h"
#include "SceneViewPanel.h"

SceneEditorCamera::SceneEditorCamera()
	: mIsViewDirty(false)
	, mYawSpeed(0.0f)
	, mPitchSpeed(0.0f)
	, mMaxPitch(Math::Pi / 3.0f)
	, mPitch(0.0f)
	, mMouseSensitivityX(500.0f)
	, mMouseSensitivityY(500.0f)
	, mForwardSpeed(0.0f)
	, mStrafeSpeed(0.0f)
	, mMaxSpeed(16.0f)
	, mMinSpeed(2.0f)
	, mSpeed(8.0f)
{
}

void SceneEditorCamera::Update()
{
	if (!mIsViewDirty) { return; }
	// SceneViewパネルにマウスが乗っていない場合、何もしない
	if (!GUIWinMain::GetSceneViewPanel()->IsMouseHovered()) { return; }
	//視点回転
	if (!Math::NearZero(mYawSpeed))
	{
		Quaternion rot = mLocalRotation;
		float angular = mYawSpeed * Time::gUnscaledDeltaTime;
		// Create quaternion for incremental rotation
		// (Rotate about up axis)
		Quaternion inc(Vector3::UnitY, angular);
		// Concatenate old and new quaternion
		rot = Quaternion::Concatenate(rot, inc);
		SetLocalRotation(rot);
	}

	// カメラの位置はオーナーの位置
	Vector3 cameraPos = mPosition;

	// ピッチ速度に基づいてピッチを更新
	mPitch += mPitchSpeed * Time::gUnscaledDeltaTime;
	// クランプピッチを[-max, +max]に制限
	mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);
	// オーナーの右ベクトルを軸とするピッチ回転を表す四元数を作成
	Quaternion q(GetRight(), mPitch);

	// 所有者をピッチクォータニオンで前方に回転
	Vector3 viewForward = Vector3::Transform(
		GetForward(), q);
	// 視線の前方100ユニットのターゲット位置。
	Vector3 target = cameraPos + viewForward * 100.0f;
	// ピッチクォータニオンを回転。
	Vector3 up = Vector3::Transform(Vector3::UnitY, q);

	// マトリックスを作成し、ビューとして設定
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	WindowRenderProperty::SetViewEye(cameraPos);
	WindowRenderProperty::SetViewTarget(target);
	WindowRenderProperty::SetViewUp(up);
	mViewMatrix = view;

	//X、Z移動処理
	// 前後移動、左右移動
	if (!Math::NearZero(mForwardSpeed) || !Math::NearZero(mStrafeSpeed))
	{
		Vector3 pos = mLocalPosition;
		pos += viewForward * mForwardSpeed * Time::gUnscaledDeltaTime;
		pos += GetRight() * mStrafeSpeed * Time::gUnscaledDeltaTime;
		SetLocalPosition(pos);
		ComputeWorldTransform();
	}
	
}

void SceneEditorCamera::ProcessInput(const struct InputState& keyState)
{
	//マウス入力
	mYawSpeed = 0;
	mPitchSpeed = 0;
	mIsViewDirty = false;
	// SceneViewパネルにマウスが乗っていない場合、何もしない
	if (!GUIWinMain::GetSceneViewPanel()->IsMouseHovered()) { return; }
	// 右クリックが押された瞬間、相対モードに切り替え
	if (keyState.Mouse.GetButtonDown(SDL_BUTTON_RIGHT))
	{
		InputSystem::RelativeMouseMode();
	}
	// 右クリックが押されていない場合、何もしない
	if(keyState.Mouse.GetButton(SDL_BUTTON_RIGHT))
	{
		mIsViewDirty = true;
		//SDLでマウスの移動数値を取得
		float x, y;
		SDL_GetRelativeMouseState(&x, &y);
		//マウスでの最大移動数値を設定

		// 最大速度での回転/秒
		const float maxAngularSpeed = Math::Pi * MAX_YAW_SPEED;
		float angularSpeed = 0.0f;
		if (x != 0)
		{
			// [-1.0, 1.0]に変換する
			angularSpeed = static_cast<float>(x) / mMouseSensitivityX;
			// 回転/秒で掛ける
			angularSpeed *= maxAngularSpeed;
		}
		mYawSpeed = angularSpeed;

		// 音高を計算する
		const float maxPitchSpeed = Math::Pi * MAX_YAW_SPEED;
		float pitchSpeed = 0.0f;
		if (y != 0)
		{
			// [-1.0, 1.0]に変換する
			pitchSpeed = static_cast<float>(y) / mMouseSensitivityY;
			pitchSpeed *= maxPitchSpeed;
		}
		mPitchSpeed = pitchSpeed;
	}
	if (keyState.Mouse.GetButtonUp(SDL_BUTTON_RIGHT))
	{
		InputSystem::AbsoluteMouseMode();
	}

	Vector2 wheel = keyState.Mouse.GetScrollWheel();

	mSpeed += wheel.y;
	if (mSpeed >= mMaxSpeed)
	{
		mSpeed = mMaxSpeed;
	}
	else if (mSpeed <= mMinSpeed)
	{
		mSpeed = mMinSpeed;
	}

	// キーボード入力
	float forwardSpeed = 0.0f;
	float strafeSpeed = 0.0f;
	// wasd movement
	if (keyState.Keyboard.GetKey(SDL_SCANCODE_W))
	{
		forwardSpeed += mSpeed;
	}
	if (keyState.Keyboard.GetKey(SDL_SCANCODE_S))
	{
		forwardSpeed -= mSpeed;
	}
	if (keyState.Keyboard.GetKey(SDL_SCANCODE_A))
	{
		strafeSpeed -= mSpeed;
	}
	if (keyState.Keyboard.GetKey(SDL_SCANCODE_D))
	{
		strafeSpeed += mSpeed;
	}
	mForwardSpeed = forwardSpeed;
	mStrafeSpeed = strafeSpeed;
}