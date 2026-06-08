#include "SceneEditorCamera.h"
#include "SceneViewPanel.h"
#include "WindowRenderProperty.h"


SceneEditorCamera::SceneEditorCamera(BaseScene* scene)
	: ActorObject(scene)
	, mMode(EditCameraMode::Null)
	, mYawSpeed(0.0f)
	, mPitchSpeed(0.0f)
	, mMaxPitch(Math::Pi / 3.0f)
	, mPitch(0.0f)
	, mMouseSensitivityX(500.0f)
	, mMouseSensitivityY(500.0f)
	, mForwardSpeed(0.0f)
	, mStrafeSpeed(0.0f)
	, mUpSpeed(0.0f)
	, mMaxSpeed(16.0f)
	, mMinSpeed(2.0f)
	, mSpeed(8.0f)
{
}

void SceneEditorCamera::Update()
{
	if (mMode == EditCameraMode::MiddleOperation)
	{
		// カメラの位置はオーナーの位置
		Vector3 cameraPos = mTransform->GetPosition();
		// オーナーの右ベクトルを軸とするピッチ回転を表す四元数を作成
		Quaternion q(mTransform->GetRight(), mPitch);
		// 所有者をピッチクォータニオンで前方に回転
		Vector3 viewForward = Vector3::Transform(mTransform->GetForward(), q);
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
		// 前後移動、左右移動
		Vector3 pos = mTransform->GetLocalPosition();
		pos += mTransform->GetRight() * mStrafeSpeed * Time::gUnscaledDeltaTime;
		pos += up * mUpSpeed * Time::gUnscaledDeltaTime;
		mTransform->SetLocalPosition(pos);
		mTransform->ComputeWorldTransform();
	}
	else if (mMode == EditCameraMode::RightOperation)
	{
		// SceneViewパネルにマウスが乗っていない場合、何もしない
		if (mSceneViewPanel != nullptr && !mSceneViewPanel->IsMouseHovered()) { return; }
		//視点回転
		if (!Math::NearZero(mYawSpeed))
		{
			Quaternion rot = mTransform->GetLocalRotation();
			float angular = mYawSpeed * Time::gUnscaledDeltaTime;
			// Create quaternion for incremental rotation
			// (Rotate about up axis)
			Quaternion inc(Vector3::UnitY, angular);
			// Concatenate old and new quaternion
			rot = Quaternion::Concatenate(rot, inc);
			mTransform->SetLocalRotation(rot);
		}
		// ピッチ速度に基づいてピッチを更新
		mPitch += mPitchSpeed * Time::gUnscaledDeltaTime;
		// クランプピッチを[-max, +max]に制限
		mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);
	}

	// カメラの位置はオーナーの位置
	Vector3 cameraPos = mTransform->GetPosition();
	// オーナーの右ベクトルを軸とするピッチ回転を表す四元数を作成
	Quaternion q(mTransform->GetRight(), mPitch);
	// 所有者をピッチクォータニオンで前方に回転
	Vector3 viewForward = Vector3::Transform(mTransform->GetForward(), q);
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
	if (mMode != EditCameraMode::MiddleOperation)
	{
		//X、Z移動処理
		// 前後移動、左右移動
		if (!Math::NearZero(mForwardSpeed) || !Math::NearZero(mStrafeSpeed))
		{
			Vector3 pos = mTransform->GetLocalPosition();
			pos += viewForward * mForwardSpeed * Time::gUnscaledDeltaTime;
			pos += mTransform->GetRight() * mStrafeSpeed * Time::gUnscaledDeltaTime;
			mTransform->SetLocalPosition(pos);
			mTransform->ComputeWorldTransform();
		}
	}
}

void SceneEditorCamera::ProcessInput(const struct InputState& keyState)
{
	//マウス入力
	mYawSpeed = 0;
	mPitchSpeed = 0;
	mStrafeSpeed = 0;
	mUpSpeed = 0;
	mForwardSpeed = 0;
	mMode = EditCameraMode::Null;
	// SceneViewパネルにマウスが乗っていない場合、何もしない
	if (mSceneViewPanel != nullptr && !mSceneViewPanel->IsMouseHovered()) { return; }
	//ホイールクリック処理
	if (keyState.Mouse.GetButtonDown(SDL_BUTTON_MIDDLE))
	{
		InputSystem::RelativeMouseMode();
		float x, y;
		SDL_GetRelativeMouseState(&x, &y);
	}
	// ホイールクリックが押されていない場合、何もしない
	if (keyState.Mouse.GetButton(SDL_BUTTON_MIDDLE))
	{
		MiddleClickViewInput(keyState);
	}
	if (keyState.Mouse.GetButtonUp(SDL_BUTTON_MIDDLE))
	{
		InputSystem::AbsoluteMouseMode();
	}

	// 右クリックが押された瞬間、相対モードに切り替え
	if (keyState.Mouse.GetButtonDown(SDL_BUTTON_RIGHT))
	{
		InputSystem::RelativeMouseMode();
		float x, y;
		SDL_GetRelativeMouseState(&x, &y);
	}
	// 右クリックが押されていない場合、何もしない
	if(keyState.Mouse.GetButton(SDL_BUTTON_RIGHT))
	{
		RightClickViewInput(keyState);
		if (keyState.Keyboard.GetKey(SDL_SCANCODE_W))
		{
			mForwardSpeed += mSpeed;
		}
		if (keyState.Keyboard.GetKey(SDL_SCANCODE_S))
		{
			mForwardSpeed -= mSpeed;
		}
		if (keyState.Keyboard.GetKey(SDL_SCANCODE_A))
		{
			mStrafeSpeed -= mSpeed;
		}
		if (keyState.Keyboard.GetKey(SDL_SCANCODE_D))
		{
			mStrafeSpeed += mSpeed;
		}
	}
	if (keyState.Mouse.GetButtonUp(SDL_BUTTON_RIGHT))
	{
		InputSystem::AbsoluteMouseMode();
	}

	//ホイール入力を取得
	Vector2 wheel = keyState.Mouse.GetScrollWheel();
	//「右クリック中でない」かつ「ホイール入力がある」場合
	if (!keyState.Mouse.GetButton(SDL_BUTTON_RIGHT) && wheel.y != 0)
	{
		//mSpeedを基準にズーム(前後移動)を行う
		float zoomSensitivity = 2.0f;
		//mForwardSpeedに値を設定
		mForwardSpeed = wheel.y * mSpeed * zoomSensitivity;
	}
}

void SceneEditorCamera::MiddleClickViewInput(const InputState& keyState)
{
	if (mMode != EditCameraMode::Null) { return; }
	mMode = EditCameraMode::MiddleOperation;

	//SDLでマウスの移動数値を取得
	float x, y;
	SDL_GetRelativeMouseState(&x, &y);

	float sensitivity = 1.0f;

	mStrafeSpeed = -x * sensitivity;
	mUpSpeed = y * sensitivity;
}

void SceneEditorCamera::RightClickViewInput(const struct InputState& keyState)
{
	if (mMode != EditCameraMode::Null) { return; }
	mMode = EditCameraMode::RightOperation;
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

	// ホイール入力で移動速度を調整
	Vector2 wheel = keyState.Mouse.GetScrollWheel();
	mSpeed += wheel.y;
	// スピードのクランプ
	mSpeed = Math::Clamp(mSpeed, mMinSpeed, mMaxSpeed);
}
