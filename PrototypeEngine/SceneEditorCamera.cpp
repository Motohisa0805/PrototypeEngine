#include "SceneEditorCamera.h"
#include "SceneViewPanel.h"

SceneEditorCamera::SceneEditorCamera()
	: mAngularSpeed(0.0f)
	, mPitchSpeed(0.0f)
	, mMaxPitch(Math::Pi / 3.0f)
	, mPitch(0.0f)
	, mHorizontalMouseSpeed(500.0f)
	, mVertexMouseSpeed(500.0f)
	, mLastMouseX(0.0f)
	, mLastMouseY(0.0f)
	, mForwardSpeed(0.0f)
	, mStrafeSpeed(0.0f)
{
	//カメラの位置を変更
}

void SceneEditorCamera::Update()
{
	// SceneViewパネルにマウスが乗っていない場合、何もしない
	if (!GUIWinMain::GetSceneViewPanel()->IsMouseHovered()) { return; }
	//視点回転
	if (!Math::NearZero(mAngularSpeed))
	{
		Quaternion rot = mLocalRotation;
		mAngular = mAngularSpeed * Time::gUnscaledDeltaTime;
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
	mPitch += mPitchSpeed * Time::gUnscaledDeltaTime;
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

	//X、Z移動処理
	// 前後移動、左右移動
	if (!Math::NearZero(mForwardSpeed) || !Math::NearZero(mStrafeSpeed))
	{
		Vector3 pos = mLocalPosition;
		pos += viewForward * mForwardSpeed * Time::gUnscaledDeltaTime;
		pos += GetRight() * mStrafeSpeed * Time::gUnscaledDeltaTime;
		mLocalPosition = pos;
	}
	
	ComputeWorldTransform();
}

void SceneEditorCamera::ProcessInput(const struct InputState& keyState)
{
	//マウス入力
	mAngularSpeed = 0;
	mPitchSpeed = 0;
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
		//SDLでマウスの移動数値を取得
		float x, y;
		SDL_GetRelativeMouseState(&x, &y);
		//マウスでの最大移動数値を設定

		// 最大速度での回転/秒
		const float maxAngularSpeed = Math::Pi * 50;
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
		const float maxPitchSpeed = Math::Pi * 50;
		float pitchSpeed = 0.0f;
		if (y != 0)
		{
			// [-1.0, 1.0]に変換する
			pitchSpeed = static_cast<float>(y) / mVertexMouseSpeed;
			pitchSpeed *= maxPitchSpeed;
		}
		mPitchSpeed = pitchSpeed;
	}
	if (keyState.Mouse.GetButtonUp(SDL_BUTTON_RIGHT))
	{
		InputSystem::AbsoluteMouseMode();
	}

	// キーボード入力
	float forwardSpeed = 0.0f;
	float strafeSpeed = 0.0f;
	// wasd movement
	if (keyState.Keyboard.GetKey(SDL_SCANCODE_W))
	{
		forwardSpeed += 8.0f;
	}
	if (keyState.Keyboard.GetKey(SDL_SCANCODE_S))
	{
		forwardSpeed -= 8.0f;
	}
	if (keyState.Keyboard.GetKey(SDL_SCANCODE_A))
	{
		strafeSpeed -= 8.0f;
	}
	if (keyState.Keyboard.GetKey(SDL_SCANCODE_D))
	{
		strafeSpeed += 8.0f;
	}
	mForwardSpeed = forwardSpeed;
	mStrafeSpeed = strafeSpeed;
}