#include "PhysWorld.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "Collider.h"
#include "Actor.h"
#include <SDL3/SDL.h>

PhysWorld::PhysWorld(BaseScene* game)
	:mGame(game)
{
}

bool PhysWorld::RayCast(const LineSegment& l, CollisionInfo& outColl, int tag)
{
	bool collided = false;
	float closestT = Math::Infinity;
	Vector3 norm = Vector3::Zero;

	for (auto collider : mCollider)
	{
		if (tag != -1 && (int)collider->GetOwner()->GetActorTag() != tag)
			continue;

		OBB obb = collider->GetWorldOBB();
		float t = 0;
		if (OnRayCastCollision(l, obb, t, norm))
		{
			if (t >= 0.0f && t < closestT)
			{
				closestT = t;
				outColl.mPoint = l.PointOnSegment(t);
				outColl.mNormal = norm;
				outColl.mCollider = collider;
				outColl.mActor = collider->GetOwner();
				collided = true;
			}
		}
	}
	return collided;
}

std::vector<PhysWorld::CollisionInfo> PhysWorld::RayCastAll(const LineSegment& l)
{
	std::vector<CollisionInfo> results;

	for (auto collider : mCollider)
	{
		float t = 0.0f;
		Vector3 norm;
		if (OnCollision(l, collider->GetWorldAABBFromOBB(), t, norm))
		{
			if (t >= 0.0f && t <= 1.0f)
			{
				CollisionInfo info;
				info.mT = t;
				info.mPoint = l.PointOnSegment(t);
				info.mNormal = norm;
				info.mCollider = collider;
				info.mActor = collider->GetOwner();
				results.push_back(info);
			}
		}
	}

	// t値（手前順）でソートする
	std::sort(results.begin(), results.end(),
		[](const CollisionInfo& a, const CollisionInfo& b)
		{
			return a.mT < b.mT;
		});

	return results;
}

void PhysWorld::SweepAndPruneXYZ()
{
	// まずX軸でソート
	std::sort(mColliderXAxis.begin(), mColliderXAxis.end(),
		[](Collider* a, Collider* b) {
			return a->GetWorldAABBFromOBB().mMin.x < b->GetWorldAABBFromOBB().mMin.x;
		});

	mCurrentHitPairs.clear();

	// X軸スイープ開始
	for (size_t i = 0; i < mColliderXAxis.size(); ++i)
	{
		Collider* colliderA = mColliderXAxis[i];
		const AABB& aabbA = colliderA->GetWorldAABBFromOBB();
		//const AABB& aabbA = colliderA->GetWorldBox();

		for (size_t j = i + 1; j < mColliderXAxis.size(); ++j)
		{
			Collider* colliderB = mColliderXAxis[j];
			const AABB& aabbB = colliderB->GetWorldAABBFromOBB();
			//const AABB& aabbB = colliderB->GetWorldBox();

			const float contactOffset = 0.001f;

			const float contactOffsetA = colliderA->GetContactOffset();
			const float contactOffsetB = colliderB->GetContactOffset();

			// X軸の最大と最小が交差してなかったらbreak（高速化）
			// ※ contactOffset を加味して比較
			if (aabbB.mMin.x - contactOffsetB > aabbA.mMax.x + contactOffsetA)
			{
				break;
			}

			// Y軸とZ軸の交差判定にも contactOffset を考慮
			if (aabbA.mMax.y + contactOffsetA < aabbB.mMin.y - contactOffsetB ||
				aabbA.mMin.y - contactOffsetA > aabbB.mMax.y + contactOffsetB)
			{
				continue;
			}

			if (aabbA.mMax.z + contactOffsetA < aabbB.mMin.z - contactOffsetB ||
				aabbA.mMin.z - contactOffsetA > aabbB.mMax.z + contactOffsetB)
			{
				continue;
			}

			// ここまで来たらAとBはAABB上は当たっている（各コライダーで精密判定を入れる）
			if (!IsOnCollision(colliderA,colliderB))
			{
				continue;// SAT上は衝突していないのでスキップ
			}

			// ここまで来たらAとBは当たっている
			auto actorA = colliderA->GetOwner();
			auto actorB = colliderB->GetOwner();

			std::pair<ActorObject*, ActorObject*> sortedPair = actorA < actorB ?
				std::make_pair(actorA, actorB) : std::make_pair(actorB, actorA);

			mCurrentHitPairs.emplace(sortedPair);

			// Enter or Stay判定
			if (mPrevHitPairs.count(sortedPair))
			{
				actorA->OnCollisionStay(actorB);
				actorB->OnCollisionStay(actorA);
				if (colliderA->IsCollider() && colliderB->IsCollider())
				{
					//当たり続けている時も判定
					if (!colliderA->IsStaticObject() && colliderB->IsStaticObject())
					{
						FixCollisions(colliderA, colliderB);
					}
					else if (colliderA->IsStaticObject() && !colliderB->IsStaticObject())
					{
						FixCollisions(colliderB, colliderA);
					}
				}
			}
			else
			{
				actorA->OnCollisionEnter(actorB);
				actorB->OnCollisionEnter(actorA);

				if (colliderA->IsCollider() && colliderB->IsCollider())
				{
					//当たり初めに判定
					if (!colliderA->IsStaticObject() && colliderB->IsStaticObject())
					{
						FixCollisions(colliderA, colliderB);
					}
					else if (colliderA->IsStaticObject() && !colliderB->IsStaticObject())
					{
						FixCollisions(colliderB, colliderA);
					}
				}
			}
		}
	}

	// Exitチェック
	for (const auto& pair : mPrevHitPairs)
	{
		if (mCurrentHitPairs.count(pair) == 0)
		{
			auto actorA = pair.first;
			auto actorB = pair.second;
			actorA->OnCollisionExit(actorB);
			actorB->OnCollisionExit(actorA);
		}
	}

	// 状態更新
	mPrevHitPairs = mCurrentHitPairs;
}

void PhysWorld::FixCollisions(class Collider* dynamicCollider, class Collider* staticCollider)
{
	std::vector<ContactPoint> contactPoints;
	const float contactOffset = dynamicCollider->GetContactOffset() + staticCollider->GetContactOffset();

	// OBB対応の接触点収集
	IsCollectContactPoints(dynamicCollider, staticCollider, contactPoints, contactOffset);

	if (contactPoints.empty())
	{
		return;
	}

	// 総押し出しベクトル（複数法線合成）
	Vector3 totalPush = Vector3::Zero;
	for (auto& contact : contactPoints)
	{
		Vector3 push = contact.normal * (contact.penetration + contactOffset);
		totalPush += push;
	}

	// 押し出し方向の正規化（あまりに小さいときはスキップ）
	if (totalPush.Length() > 0.0001f)
	{
		totalPush = totalPush.Normalized() * contactPoints[0].penetration;
	}

	// 合成ベクトルを1つの方向に正規化（複合押し出し）
	if (!Math::NearZero(totalPush.Length()))
	{
		auto actor = dynamicCollider->GetOwner();
		auto rb = actor->GetRigidbody();
		actor->SetLocalPosition(actor->GetPosition() + totalPush);
		actor->ComputeWorldTransform(actor->GetParentActor() == nullptr ? NULL : &actor->GetParentActor()->GetWorldTransform());

		// Rigidbodyに押し出し方向を通知（滑り/跳ね返り等に使用）
		if (rb)
		{
			rb->ResolveCollision(totalPush);
		}
	}
}

bool PhysWorld::IsOnCollision(Collider* colliderA, Collider* colliderB)
{
	if (colliderA->GetType() == Collider::BoxType && colliderB->GetType() == Collider::BoxType)
	{
		return OnCollision(colliderA->GetWorldOBB(), colliderB->GetWorldOBB());
	}
	else if (colliderA->GetType() == Collider::BoxType && colliderB->GetType() == Collider::SphereType)
	{
		return OnCollision(colliderA->GetWorldOBB(),colliderB->GetWorldSphere());
	}
	else if (colliderA->GetType() == Collider::SphereType && colliderB->GetType() == Collider::BoxType)
	{
		return OnCollision(colliderB->GetWorldOBB(), colliderA->GetWorldSphere());
	}
	else if (colliderA->GetType() == Collider::BoxType && colliderB->GetType() == Collider::CapsuleType)
	{
		return OnCollision(colliderA->GetWorldOBB(), colliderB->GetWorldCapsule());
	}
	else if (colliderA->GetType() == Collider::CapsuleType && colliderB->GetType() == Collider::BoxType)
	{
		return OnCollision(colliderB->GetWorldOBB(), colliderA->GetWorldCapsule());
	}


	if (colliderA->GetType() == Collider::SphereType && colliderB->GetType() == Collider::SphereType)
	{
		return OnCollision(colliderA->GetWorldSphere(), colliderB->GetWorldSphere());
	}
	/*
	else if (colliderA->GetType() == Collider::SphereType && colliderB->GetType() == Collider::BoxType)
	{
		return OnCollision(colliderB->GetWorldOBB(), colliderA->GetWorldSphere());
	}
	else if (colliderA->GetType() == Collider::BoxType && colliderB->GetType() == Collider::SphereType)
	{
		return OnCollision(colliderA->GetWorldOBB(), colliderB->GetWorldSphere());
	}
	*/
	else if (colliderA->GetType() == Collider::SphereType && colliderB->GetType() == Collider::CapsuleType)
	{
		return OnCollision(colliderA->GetWorldSphere(), colliderB->GetWorldCapsule());
	}
	else if (colliderA->GetType() == Collider::CapsuleType && colliderB->GetType() == Collider::SphereType)
	{
		return OnCollision(colliderA->GetWorldCapsule(), colliderB->GetWorldSphere());
	}


	if (colliderA->GetType() == Collider::CapsuleType && colliderB->GetType() == Collider::CapsuleType)
	{
		return OnCollision(colliderA->GetWorldCapsule(),colliderB->GetWorldCapsule());
	}

	return false;
}

void PhysWorld::IsCollectContactPoints(Collider* colliderA, Collider* colliderB, std::vector<ContactPoint>& outContacts, float contactOffset)
{
	if (colliderA->GetType() == Collider::BoxType && colliderB->GetType() == Collider::BoxType)
	{
		CollectContactPoints_OBB_OBB(colliderA->GetWorldOBB(), colliderB->GetWorldOBB(), outContacts, contactOffset);
	}
	else if (colliderA->GetType() == Collider::BoxType && colliderB->GetType() == Collider::SphereType)
	{
		CollectContactPoints_OBB_Sphere(colliderA->GetWorldOBB(), colliderB->GetWorldSphere(), outContacts, contactOffset);
	}
	else if (colliderA->GetType() == Collider::SphereType && colliderB->GetType() == Collider::BoxType)
	{
		CollectContactPoints_OBB_Sphere(colliderB->GetWorldOBB(), colliderA->GetWorldSphere(), outContacts, contactOffset);
	}
	else if (colliderA->GetType() == Collider::BoxType && colliderB->GetType() == Collider::CapsuleType)
	{
		CollectContactPoints_OBB_Capsule(colliderA->GetWorldOBB(), colliderB->GetWorldCapsule(), outContacts, contactOffset);
	}
	else if (colliderA->GetType() == Collider::CapsuleType && colliderB->GetType() == Collider::BoxType)
	{
		CollectContactPoints_OBB_Capsule(colliderB->GetWorldOBB(), colliderA->GetWorldCapsule(), outContacts, contactOffset);
	}


	if (colliderA->GetType() == Collider::SphereType && colliderB->GetType() == Collider::SphereType)
	{
		CollectContactPoints_Sphere_Sphere(colliderA->GetWorldSphere(), colliderB->GetWorldSphere(), outContacts, contactOffset);
	}
	else if (colliderA->GetType() == Collider::SphereType && colliderB->GetType() == Collider::CapsuleType)
	{
		CollectContactPoints_Sphere_Capsule(colliderA->GetWorldSphere(), colliderB->GetWorldCapsule(), outContacts, contactOffset);
	}
	else if (colliderA->GetType() == Collider::CapsuleType && colliderB->GetType() == Collider::SphereType)
	{
		CollectContactPoints_Sphere_Capsule(colliderB->GetWorldSphere(), colliderA->GetWorldCapsule(), outContacts, contactOffset);
	}


	if (colliderA->GetType() == Collider::CapsuleType && colliderB->GetType() == Collider::CapsuleType)
	{
		CollectContactPoints_Capsule_Capsule(colliderA->GetWorldCapsule(), colliderB->GetWorldCapsule(), outContacts, contactOffset);
	}
}

void PhysWorld::CollectContactPoints_OBB_OBB(const OBB& a, const OBB& b, std::vector<ContactPoint>& outContacts, float contactOffset)
{
	Vector3 normal;
	float depth;

	if (GetContactInfo_OBB(a, b, normal, depth))
	{
		outContacts.emplace_back(ContactPoint{ normal, depth });
	}
}

bool PhysWorld::GetContactInfo_OBB(const OBB& a, const OBB& b, Vector3& outNormal, float& outDepth)
{
	Vector3 aAxes[3] = 
	{
		Vector3::Transform(Vector3::UnitX, a.mRotation),
		Vector3::Transform(Vector3::UnitY, a.mRotation),
		Vector3::Transform(Vector3::UnitZ, a.mRotation)
	};

	Vector3 bAxes[3] = 
	{
		Vector3::Transform(Vector3::UnitX, b.mRotation),
		Vector3::Transform(Vector3::UnitY, b.mRotation),
		Vector3::Transform(Vector3::UnitZ, b.mRotation)
	};

	Vector3 axes[15];
	int axisCount = 0;

	for (int i = 0; i < 3; ++i)
	{
		axes[axisCount++] = aAxes[i];
	}
	for (int i = 0; i < 3; ++i)
	{
		axes[axisCount++] = bAxes[i];
	}

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			Vector3 cross = Vector3::Cross(aAxes[i], bAxes[j]);
			if (cross.LengthSq() > 1e-6f)
			{
				axes[axisCount++] = cross.Normalized();
			}
		}
	}

	float minOverlap = Math::Infinity;
	Vector3 bestAxis = Vector3::Zero;

	for (int i = 0; i < axisCount; ++i)
	{
		Vector3 axis = axes[i];
		float minA, maxA, minB, maxB;
		ProjectOBB(a, axis, minA, maxA);
		ProjectOBB(b, axis, minB, maxB);

		float overlap = std::min(maxA, maxB) - std::max(minA, minB);
		if (overlap <= 0.0f)
		{
			// 分離軸あり
			return false; 
		}

		if (overlap < minOverlap)
		{
			minOverlap = overlap;
			bestAxis = axis;
		}
	}

	// より信頼できる方向を使う（localからtransformされた差ベクトル）
	Vector3 dir = a.mCenter - b.mCenter;
	float fDir = Vector3::Dot(dir, bestAxis);
	if (fDir < 0.0f)
	{
		bestAxis *= -1.0f;
	}

	outNormal = bestAxis;
	outDepth = minOverlap;
	return true;
}

void PhysWorld::CollectContactPoints_Sphere_Sphere(const Sphere& a, const Sphere& b, std::vector<ContactPoint>& outContacts, float contactOffset)
{
	Vector3 diff = b.mCenter - a.mCenter;
	float dist = diff.Length();

	float radiusSum = a.mRadius + b.mRadius;
	float penetration = radiusSum - dist;

	if (penetration + contactOffset > 0.0f)
	{
		Vector3 normal = (dist > 0.0001f) ? diff / dist : Vector3::UnitX;
		outContacts.emplace_back(ContactPoint{ normal, penetration });
	}
}

void PhysWorld::CollectContactPoints_Capsule_Capsule(const Capsule& a, const Capsule& b, std::vector<ContactPoint>& outContacts, float contactOffset)
{
	float distSq = LineSegment::MinDistSq(a.mSegment, b.mSegment);
	float radiusSum = a.mRadius + b.mRadius;

	if (distSq <= (radiusSum + contactOffset) * (radiusSum + contactOffset))
	{
		float dist = std::sqrt(distSq);
		float penetration = radiusSum - dist;

		// 最近接点を計算
		Vector3 pa, pb;
		ClosestPtsBetweenSegments(a.mSegment, b.mSegment, pa, pb); // 最近接点計算

		Vector3 normal = pb - pa;
		if (normal.LengthSq() > 0.0001f)
			normal.Normalize();
		else
			normal = Vector3::UnitX;

		outContacts.emplace_back(ContactPoint{ normal, penetration });
	}
}

void PhysWorld::CollectContactPoints_OBB_Sphere(const OBB& a, const Sphere& b, std::vector<ContactPoint>& outContacts, float contactOffset)
{
	Vector3 closest = ClosestPointOnOBB(b.mCenter, a);
	Vector3 diff = b.mCenter - closest;
	float distSq = diff.LengthSq();
	float radius = b.mRadius;

	if (distSq <= (radius + contactOffset) * (radius + contactOffset))
	{
		float dist = std::sqrt(distSq);
		float penetration = radius - dist;

		Vector3 normal = (dist > 0.0001f) ? diff / dist : Vector3::UnitX;

		// normal を押し出される Sphere に外向きに揃える
		normal *= -1.0f;
		outContacts.emplace_back(ContactPoint{ normal, penetration });
	}
}

void PhysWorld::CollectContactPoints_OBB_Capsule(const OBB& a, const Capsule& b, std::vector<ContactPoint>& outContacts, float contactOffset)
{
	const int steps = 10;
	float minDistSq = Math::Infinity;
	Vector3 bestPointOnSeg, bestPointOnOBB;

	for (int i = 0; i <= steps; ++i)
	{
		float t = i / static_cast<float>(steps);
		Vector3 pointOnSeg = Vector3::Lerp(b.mSegment.mStart, b.mSegment.mEnd, t);
		Vector3 pointOnOBB = ClosestPointOnOBB(pointOnSeg, a);

		float distSq = (pointOnSeg - pointOnOBB).LengthSq();
		if (distSq < minDistSq)
		{
			minDistSq = distSq;
			bestPointOnSeg = pointOnSeg;
			bestPointOnOBB = pointOnOBB;
		}
	}

	float radius = b.mRadius;
	if (minDistSq <= (radius + contactOffset) * (radius + contactOffset))
	{
		float dist = std::sqrt(minDistSq);
		float penetration = radius - dist;

		Vector3 normal = bestPointOnSeg - bestPointOnOBB;
		if (normal.LengthSq() > 0.0001f)
			normal.Normalize();
		else
			normal = Vector3::UnitX;

		normal *= -1.0f;

		outContacts.emplace_back(ContactPoint{ normal, penetration });
	}
}

void PhysWorld::CollectContactPoints_Sphere_Capsule(const Sphere& a, const Capsule& b, std::vector<ContactPoint>& outContacts, float contactOffset)
{
	float distSq = b.mSegment.MinDistSq(a.mCenter);
	float radiusSum = a.mRadius + b.mRadius;

	if (distSq <= (radiusSum + contactOffset) * (radiusSum + contactOffset))
	{
		Vector3 ab = b.mSegment.mEnd - b.mSegment.mStart;
		float abLenSq = ab.LengthSq();

		float t = Vector3::Dot(a.mCenter - b.mSegment.mStart, ab) / abLenSq;
		t = Math::Clamp(t, 0.0f, 1.0f);

		Vector3 closest = b.mSegment.mStart + ab * t;
		Vector3 diff = a.mCenter - closest;
		float dist = std::sqrt(distSq);
		float penetration = radiusSum - dist;

		Vector3 normal = (dist > 0.0001f) ? diff / dist : Vector3::UnitX;
		outContacts.emplace_back(ContactPoint{ normal, penetration });
	}
}


void PhysWorld::AddCollider(Collider* box)
{
	mCollider.push_back(box);
	mColliderXAxis.emplace_back(box);
	mColliderYAxis.emplace_back(box);
	mColliderZAxis.emplace_back(box);
}

void PhysWorld::RemoveCollider(Collider* box)
{
	auto iter = std::find(mCollider.begin(), mCollider.end(), box);
	if (iter != mCollider.end())
	{
		// ベクトルの末尾にスワップし、
		// ポップオフします（コピーの消去を避けるため）
		std::iter_swap(iter, mCollider.end() - 1);
		mCollider.pop_back();
	}

	iter = std::find(mColliderXAxis.begin(), mColliderXAxis.end(), box);
	if (iter != mColliderXAxis.end())
	{
		// ベクトルの末尾にスワップし、
		// ポップオフします（コピーの消去を避けるため）
		std::iter_swap(iter, mColliderXAxis.end() - 1);
		mColliderXAxis.pop_back();
	}

	iter = std::find(mColliderYAxis.begin(), mColliderYAxis.end(), box);
	if (iter != mColliderYAxis.end())
	{
		// ベクトルの末尾にスワップし、
		// ポップオフします（コピーの消去を避けるため）
		std::iter_swap(iter, mColliderYAxis.end() - 1);
		mColliderYAxis.pop_back();
	}

	iter = std::find(mColliderZAxis.begin(), mColliderZAxis.end(), box);
	if (iter != mColliderZAxis.end())
	{
		// ベクトルの末尾にスワップし、
		// ポップオフします（コピーの消去を避けるため）
		std::iter_swap(iter, mColliderZAxis.end() - 1);
		mColliderZAxis.pop_back();
	}

	for (auto it = mPrevHitPairs.begin(); it != mPrevHitPairs.end(); )
	{
		if (it->first == box->GetOwner() || it->second == box->GetOwner())
		{
			it = mPrevHitPairs.erase(it); // eraseは次のイテレータを返す
		}
		else
		{
			++it;
		}
	}
}
