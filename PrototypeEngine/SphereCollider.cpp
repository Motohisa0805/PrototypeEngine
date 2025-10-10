#include "SphereCollider.h"

SphereCollider::SphereCollider(ActorObject* owner, int updateOrder)
	:Collider(owner, updateOrder)
	, mObjectSphere(Vector3::Zero, 0)
	, mWorldSphere(Vector3::Zero, 0)
	, mShouldRotate(true)
{
	mName = "SphereCollider";
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::OnUpdateWorldTransform()
{
	mWorldSphere = mObjectSphere;

	Vector3 scale = mOwner->GetLocalScale();
	float maxScale = std::max({ scale.x, scale.y, scale.z });
	
	mWorldSphere.mRadius = mObjectSphere.mRadius * maxScale;

	mWorldSphere.mCenter = mObjectSphere.mCenter * maxScale + mOwner->GetPosition();
}

OBB SphereCollider::GetWorldOBB() const
{
	return mWorldOBB;
}

AABB SphereCollider::GetWorldAABBFromOBB() const
{
	Vector3 rVec(mWorldSphere.mRadius, mWorldSphere.mRadius, mWorldSphere.mRadius);
	return AABB(
		mWorldSphere.mCenter - rVec,
		mWorldSphere.mCenter + rVec
	);
}
