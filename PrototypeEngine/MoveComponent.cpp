#include "MoveComponent.h"


MoveComponent::MoveComponent(class ActorObject* owner, int updateOrder)
	:Component(owner, updateOrder)
	, mAngularSpeed(0.0f)
	, mForwardSpeed(0.0f)
{

}

void MoveComponent::Update(float deltaTime)
{
	if (!Math::NearZero(mAngularSpeed))
	{
		Quaternion rot = mOwner->GetLocalRotation();
		float angle = mAngularSpeed * deltaTime;
		// Create quaternion for incremental rotation
		// (Rotate about up axis)
		Quaternion inc(Vector3::UnitY, angle);
		// Concatenate old and new quaternion
		rot = Quaternion::Concatenate(rot, inc);
		mOwner->SetLocalRotation(rot);
	}

	if (!Math::NearZero(mForwardSpeed) || !Math::NearZero(mStrafeSpeed) || !Math::NearZero(mYSpeed))
	{
		Vector3 pos = mOwner->GetLocalPosition();
		pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
		pos += mOwner->GetRight() * mStrafeSpeed * deltaTime;
		pos += mOwner->GetUp() * mYSpeed * deltaTime;
		mOwner->SetLocalPosition(pos);
	}

}