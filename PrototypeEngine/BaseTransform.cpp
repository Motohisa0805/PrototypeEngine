#include "BaseTransform.h"


BaseTransform::BaseTransform(Entity* owner)
: Component(owner)
, mPosition(Vector3::Zero)
, mLocalPosition(Vector3::Zero)
, mPositionOffset(Vector3::Zero)
, mRotation(Quaternion::Identity)
, mLocalRotation(Quaternion::Identity)
, mScale(Vector3(1.0f, 1.0f, 1.0f))
, mLocalScale(Vector3(1.0f, 1.0f, 1.0f))
, mIsDirty(false)
, mWorldTransform()
, mLocalTransform()
{
}

BaseTransform::~BaseTransform()
{
}

void BaseTransform::RotateToNewForward(const Vector3& forward)
{
	// Figure out difference between original (unit x) and new
	float dot = Vector3::Dot(Vector3::UnitZ, forward);
	float angle = Math::Acos(dot);
	// Facing down X
	if (dot > 0.9999f)
	{
		SetLocalRotation(Quaternion::Identity);
	}
	// Facing down -X
	else if (dot < -0.9999f)
	{
		SetLocalRotation(Quaternion(Vector3::UnitZ, Math::Pi));
	}
	else
	{
		// Rotate about axis from cross product
		Vector3 axis = Vector3::Cross(Vector3::UnitZ, forward);
		axis.Normalize();
		SetLocalRotation(Quaternion(axis, angle));
	}
}

void BaseTransform::LookAt(const Vector3& targetPosition)
{
	Vector3 currentPosition = mPosition;
	Vector3 forward = (targetPosition - currentPosition).Normalized();
	Vector3 up = Vector3::UnitY;

	Quaternion rot = Quaternion::LookRotation(forward, up);
	SetLocalRotation(rot);

	mIsDirty = true;
}

void BaseTransform::SetPosition(const Vector3& pos)
{
	//ワールド座標からローカル座標を逆計算してmLocalPositionを更新
	mLocalPosition = pos;
	SetDirty();
	ComputeWorldTransform();
}

void BaseTransform::SetRotation(const Quaternion& rotation)
{
	mLocalRotation = rotation;
	SetDirty(); // 更新フラグを立てる
	ComputeWorldTransform();
}

void BaseTransform::SetScale(Vector3 scale)
{
	mLocalScale = scale;
	SetDirty();
	ComputeWorldTransform();
}