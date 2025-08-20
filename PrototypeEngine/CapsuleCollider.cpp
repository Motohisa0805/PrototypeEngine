#include "CapsuleCollider.h"

CapsuleCollider::CapsuleCollider(ActorObject* owner, int updateOrder)
	: Collider(owner,updateOrder)
	, mObjectCapsule(Vector3::Zero,Vector3::Zero,0.0f)
	, mWorldCapsule(Vector3::Zero, Vector3::Zero, 0.0f)
	, mShouldRotate(true)
{
}

CapsuleCollider::~CapsuleCollider()
{
}

void CapsuleCollider::OnUpdateWorldTransform()
{
    mWorldCapsule = mObjectCapsule;

    // スケール・回転・位置を取得
    Vector3 scale = mOwner->GetScale();
    Quaternion rot = mOwner->GetRotation();
    Vector3 pos = mOwner->GetPosition();

    // 回転＋スケール → 始点・終点をワールド変換
    Vector3 localStart = mWorldCapsule.mSegment.mStart;
    Vector3 localEnd = mWorldCapsule.mSegment.mEnd;

    Vector3 rotatedStart = rot.Rotate(localStart);
    Vector3 rotatedEnd = rot.Rotate(localEnd);

    mWorldCapsule.mSegment.mStart = rotatedStart + pos;
    mWorldCapsule.mSegment.mEnd = rotatedEnd + pos;

    // 半径スケール（等方スケール想定 or 最大軸スケール使用）
    float uniformScale = std::max({ scale.x, scale.y, scale.z });
    mWorldCapsule.mRadius = mWorldCapsule.mRadius * uniformScale;

}

OBB CapsuleCollider::GetWorldOBB() const
{
    return mWorldOBB;
}

AABB CapsuleCollider::GetWorldAABBFromOBB() const
{
    const Vector3& p0 = mWorldCapsule.mSegment.mStart;
    const Vector3& p1 = mWorldCapsule.mSegment.mEnd;
    float r = mWorldCapsule.mRadius;

    Vector3 min = Vector3::Min(p0, p1);
    Vector3 max = Vector3::Max(p0, p1);

    Vector3 rVec(r, r, r);
    return AABB(min - rVec, max + rVec);
}
