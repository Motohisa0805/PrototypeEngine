#include "PushButtonMove.h"

PushButtonMove::PushButtonMove(ActorObject* owner, int updateOrder)
	:Component(owner, updateOrder)
	, mForwardSpeed(0.0f)
{
	mSpeed = 0.5f;
}

void PushButtonMove::Update(float deltaTime)
{
	//XAZˆÚ“®ˆ—
	if (!Math::NearZero(mForwardSpeed) || !Math::NearZero(mStrafeSpeed))
	{
		Vector3 pos = mOwner->GetLocalPosition();
		pos += mOwner->GetForward() * mForwardSpeed;
		pos += mOwner->GetRight() * mStrafeSpeed;
		mOwner->SetLocalPosition(pos);
	}
}

void PushButtonMove::MoveInputUpdate(const InputState& keys)
{
	Vector3 pos;

	if (keys.Keyboard.GetKeyDown(SDL_SCANCODE_RIGHT))
	{
		pos.x += mSpeed;
	}
	else if (keys.Keyboard.GetKeyDown(SDL_SCANCODE_LEFT))
	{
		pos.x -= mSpeed;
	}

	if (keys.Keyboard.GetKeyDown(SDL_SCANCODE_UP))
	{
		pos.z += mSpeed;
	}
	else if (keys.Keyboard.GetKeyDown(SDL_SCANCODE_DOWN))
	{
		pos.z -= mSpeed;
	}
	mForwardSpeed = pos.z;
	mStrafeSpeed = pos.x;
	mJumpPower = pos.y;
}
