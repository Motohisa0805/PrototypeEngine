#include "FollowObjectMovement.h"

FollowObjectMovement::FollowObjectMovement(ActorObject* owner, int updateOrder)
	:Component(owner, updateOrder)
	, mMaxSpeed(8.0f)
	, mGravity(false)
	, mJumping(false)
	, mVelocity(Vector3::Zero)
	, mInputDirection(Vector3::Zero)
{
	mGravity = true;
}

void FollowObjectMovement::FixedUpdate(float deltaTime)
{
	//追従オブジェクトの回転処理
	//移動方向に正面を向かせる処理
	float rotationSpeed = 25.0f * deltaTime;
	Quaternion targetRotation;
	if (!Math::NearZero(mVelocity.x)|| !Math::NearZero(mVelocity.y)|| !Math::NearZero(mVelocity.z))
	{
		targetRotation = Quaternion::LookRotation(mVelocity, Vector3::UnitY);
		Quaternion currnetRotation = Quaternion::RotateTowards(mOwner->GetLocalRotation(), targetRotation, rotationSpeed);
		mOwner->SetLocalRotation(currnetRotation);
	}

	//移動処理
	// カメラの方向で前後左右を決めて移動
	// カメラの前方と右ベクトルを取得（XZ平面に投影）
	Vector3 camForward = mGame->GetCamera()->GetCameraForward();
	camForward.y = 0;
	camForward.Normalize();
	// カメラの右ベクトルを取得
	Vector3 camRight = mGame->GetCamera()->GetCameraRight();
	camRight.y = 0;
	camRight.Normalize();

	// 入力方向に基づく移動ベクトル（zが前後、xが左右）
	Vector3 moveDir = (camForward * mInputDirection.z) + (camRight * mInputDirection.x);

	if (!Math::NearZero(moveDir.LengthSq())) {
		Vector3 pos = mOwner->GetLocalPosition();
		moveDir.Normalize();
		pos += moveDir * mMaxSpeed * deltaTime;
		mOwner->SetLocalPosition(pos);
	}
}

void FollowObjectMovement::Update(float deltaTime)
{
	//※入力とカメラのyawマトリックスから移動量を取得
	Vector3 inputDir = mInputDirection;
	// カメラの回転行列を取得（yawだけ）
	// Y軸回転のみ
	Matrix4 cameraYawRot = mGame->GetCamera()->GetCameraYawRot();
	Vector3 moveDir = Vector3::Transform(inputDir, cameraYawRot);
	if (moveDir.Length() <= 0) { return; }
	mVelocity = moveDir;
}

void FollowObjectMovement::MoveInputUpdate(const InputState& keys)
{
	mInputDirection = Vector3::Zero;
	// wasd movement
	if (keys.Keyboard.GetKey(SDL_SCANCODE_W))
	{
		mInputDirection.z = 1.0f;
	}
	if (keys.Keyboard.GetKey(SDL_SCANCODE_S))
	{
		mInputDirection.z = -1.0f;
	}
	if (keys.Keyboard.GetKey(SDL_SCANCODE_A))
	{
		mInputDirection.x = -1.0f;
	}
	if (keys.Keyboard.GetKey(SDL_SCANCODE_D))
	{
		mInputDirection.x = 1.0f;
	}

	if (mGravity)
	{
		if (keys.Keyboard.GetKeyDown(SDL_SCANCODE_SPACE) && !mJumping)
		{
			if (mOwner->GetRigidbody())
			{
				mOwner->GetRigidbody()->AddForce(Vector3::UnitY * 1000.0f);
			}
		}
	}
}
