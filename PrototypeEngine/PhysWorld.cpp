#include "PhysWorld.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "Collider.h"
#include "Actor.h"
#include <SDL3/SDL.h>
#include "Time.h"

PhysWorld::PhysWorld()
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
		Vector3 norm = Vector3::Zero;
		// OBBを持つBoxColliderに対しては、RayCast(LineSegment, OBB)で判定する
		if (collider->GetType() == Collider::BoxType)
		{
			OBB obb = collider->GetWorldOBB();
			if (OnRayCastCollision(l, obb, t, norm))
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
			//TODO : のちのち他コライダーも追加
			/*
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
			*/
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
	// X軸でソート
	std::sort(mColliderXAxis.begin(), mColliderXAxis.end(),
		[](Collider* a, Collider* b) {
			return a->GetWorldAABBFromOBB().mMin.x < b->GetWorldAABBFromOBB().mMin.x;
		});

	mCurrentHitPairs.clear();


	vector<ContactManifold> manifolds;

	// X軸スイープ開始
	for (size_t i = 0; i < mColliderXAxis.size(); ++i)
	{
		Collider* colliderA = mColliderXAxis[i];
		const AABB& aabbA = colliderA->GetWorldAABBFromOBB();

		for (size_t j = i + 1; j < mColliderXAxis.size(); ++j)
		{
			Collider* colliderB = mColliderXAxis[j];
			const AABB& aabbB = colliderB->GetWorldAABBFromOBB();

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
				/*
				if (colliderA->IsCollider() && colliderB->IsCollider())
				{
					//当たり続けている時も判定
					if (colliderA->GetOwner()->GetComponent<Rigidbody>() && !colliderB->GetOwner()->GetComponent<Rigidbody>())
					{
						FixCollisions(colliderA, colliderB);
					}
					else if (!colliderA->GetOwner()->GetComponent<Rigidbody>() && colliderB->GetOwner()->GetComponent<Rigidbody>())
					{
						FixCollisions(colliderB, colliderA);
					}
				}
				*/
			}
			else
			{
				actorA->OnCollisionEnter(actorB);
				actorB->OnCollisionEnter(actorA);
				/*
				if (colliderA->IsCollider() && colliderB->IsCollider())
				{
					//当たり初めに判定
					if (colliderA->GetOwner()->GetComponent<Rigidbody>() && !colliderB->GetOwner()->GetComponent<Rigidbody>())
					{
						FixCollisions(colliderA, colliderB);
					}
					else if (!colliderA->GetOwner()->GetComponent<Rigidbody>() && colliderB->GetOwner()->GetComponent<Rigidbody>())
					{
						FixCollisions(colliderB, colliderA);
					}
				}
				*/
			}

			// 押し出し処理のためのContactManifoldを作成する
			if (colliderA->IsCollider() && colliderB->IsCollider())
			{
				if (colliderA->IsCollider() && colliderB->IsCollider())
				{
					Rigidbody* rbA = actorA->GetComponent<Rigidbody>();
					Rigidbody* rbB = actorB->GetComponent<Rigidbody>();

					// 両方ともRigidbodyがない（静止物同士）なら計算不要
					if (!rbA && !rbB) continue;

					std::vector<ContactPoint> cpList;
					if (IsCollectContactPoints(colliderA, colliderB, cpList, contactOffsetA + contactOffsetB))
					{
						ContactManifold m;

						m.gRbA = rbA;
						m.gRbB = rbB;
						m.gNormal = cpList[0].mNormal;
						/*
						// rbAが常に有効（動的）になるようにAとBを入れ替える
						if (!rbA && rbB)
						{
							m.rbA = nullptr;
							m.rbB = rbB; // 静止物として扱う
							// AとBを入れ替えたので、法線ベクトルも「逆向き」にする
							m.normal = cpList[0].normal;
						}
						else if (rbA && !rbB)
						{
							m.rbA = rbA;
							m.rbB = nullptr; // 静止物として扱う
							// AとBを入れ替えたので、法線ベクトルも「逆向き」にする
							m.normal = -1.0f * cpList[0].normal;
						}
						else
						{
							// 両方動的、またはAだけ動的な場合はそのまま
							m.rbA = rbA;
							m.rbB = rbB;
							m.normal = cpList[0].normal;
						}
						*/

						m.gPenetration = cpList[0].mPenetration;
						for (auto& cp : cpList) m.gContactPoints.push_back(cp.mPosition);

						manifolds.push_back(m);
					}
				}
			}
		}
	}
	// 集めた全ペアに対して、反復計算（ソルバー）を実行する
	// ここで初めて物体が動かされる。1フレームに1回だけこの関数を呼ぶ。
	ApplyIterations(manifolds, Time::gDeltaTime);

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
	Vector3 totalNormal = Vector3::Zero;
	float maxPenetration = 0.0f; // 最大めり込み深さを追跡
	for (auto& contact : contactPoints)
	{
		totalNormal += contact.mNormal; // 法線を単純に合成して方向を求める
		if (contact.mPenetration > maxPenetration)
		{
			maxPenetration = contact.mPenetration; // 最も深いめり込みを記録
		}
	}
	// 押し出し方向の正規化と、最大めり込み深さによる押し出し量の決定
	Vector3 totalPush = Vector3::Zero;
	// 押し出し方向の正規化（あまりに小さいときはスキップ）
	if (totalNormal.Length() > 0.0001f)
	{
		totalPush = totalNormal.Normalized() * maxPenetration;
	}

	// 合成ベクトルを1つの方向に正規化（複合押し出し）
	if (!Math::NearZero(totalPush.Length()))
	{
		auto actor = dynamicCollider->GetOwner();
		auto rb = actor->GetComponent<Rigidbody>();
		actor->GetTransform()->SetLocalPosition(actor->GetTransform()->GetPosition() + totalPush);
		actor->GetTransform()->ComputeWorldTransform();

		// Rigidbodyに押し出し方向を通知（滑り/跳ね返り等に使用）
		if (rb)
		{
			for (auto& contact : contactPoints)
			{
				rb->ResolveCollision(contact.mNormal, contact.mPosition,contact.mPenetration);
			}
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

bool PhysWorld::IsCollectContactPoints(Collider* colliderA, Collider* colliderB, std::vector<ContactPoint>& outContacts, float contactOffset)
{
	Collider* cA = colliderA;
	Collider* cB = colliderB;
	bool swapped = false;

	// 型の順序を強制する (例: Box < Sphere < Capsule)
	// こうすることで (Sphere, Box) という組み合わせを (Box, Sphere) として扱える
	if (cA->GetType() > cB->GetType())
	{
		std::swap(cA, cB);
		swapped = true;
	}

	bool result = false;
	Collider::ColliderType typeA = cA->GetType();
	Collider::ColliderType typeB = cB->GetType();

	//Colliderの型の組み合わせに応じて、適切な接触点収集関数を呼び出す
	if (typeA == Collider::BoxType)
	{
		if (typeB == Collider::BoxType)
			result = CollectContactPoints_OBB_OBB(cA->GetWorldOBB(), cB->GetWorldOBB(), outContacts, contactOffset);
		else if (typeB == Collider::SphereType)
			result = CollectContactPoints_OBB_Sphere(cA->GetWorldOBB(), cB->GetWorldSphere(), outContacts, contactOffset);
		else if (typeB == Collider::CapsuleType)
			result = CollectContactPoints_OBB_Capsule(cA->GetWorldOBB(), cB->GetWorldCapsule(), outContacts, contactOffset);
	}
	else if (typeA == Collider::SphereType)
	{
		if (typeB == Collider::SphereType)
			result = CollectContactPoints_Sphere_Sphere(cA->GetWorldSphere(), cB->GetWorldSphere(), outContacts, contactOffset);
		else if (typeB == Collider::CapsuleType)
			result = CollectContactPoints_Sphere_Capsule(cA->GetWorldSphere(), cB->GetWorldCapsule(), outContacts, contactOffset);
	}
	else if(typeA == Collider::CapsuleType)
	{
		if (typeB == Collider::CapsuleType)
			result = CollectContactPoints_Capsule_Capsule(cA->GetWorldCapsule(), cB->GetWorldCapsule(), outContacts, contactOffset);
	}

	if (result && swapped)
	{
		for (auto& cp : outContacts)
		{
			cp.mNormal = -1.0f * cp.mNormal;
		}
	}
	return result;

	/*
	if (colliderA->GetType() == Collider::BoxType && colliderB->GetType() == Collider::BoxType)
	{
		return CollectContactPoints_OBB_OBB(colliderA->GetWorldOBB(), colliderB->GetWorldOBB(), outContacts, contactOffset);
	}
	else if (colliderA->GetType() == Collider::BoxType && colliderB->GetType() == Collider::SphereType)
	{
		return CollectContactPoints_OBB_Sphere(colliderA->GetWorldOBB(), colliderB->GetWorldSphere(), outContacts, contactOffset);
	}
	else if (colliderA->GetType() == Collider::SphereType && colliderB->GetType() == Collider::BoxType)
	{
		return CollectContactPoints_OBB_Sphere(colliderB->GetWorldOBB(), colliderA->GetWorldSphere(), outContacts, contactOffset);
	}
	else if (colliderA->GetType() == Collider::BoxType && colliderB->GetType() == Collider::CapsuleType)
	{
		return CollectContactPoints_OBB_Capsule(colliderA->GetWorldOBB(), colliderB->GetWorldCapsule(), outContacts, contactOffset);
	}
	else if (colliderA->GetType() == Collider::CapsuleType && colliderB->GetType() == Collider::BoxType)
	{
		return CollectContactPoints_OBB_Capsule(colliderB->GetWorldOBB(), colliderA->GetWorldCapsule(), outContacts, contactOffset);
	}


	if (colliderA->GetType() == Collider::SphereType && colliderB->GetType() == Collider::SphereType)
	{
		return CollectContactPoints_Sphere_Sphere(colliderA->GetWorldSphere(), colliderB->GetWorldSphere(), outContacts, contactOffset);
	}
	else if (colliderA->GetType() == Collider::SphereType && colliderB->GetType() == Collider::CapsuleType)
	{
		return CollectContactPoints_Sphere_Capsule(colliderA->GetWorldSphere(), colliderB->GetWorldCapsule(), outContacts, contactOffset);
	}
	else if (colliderA->GetType() == Collider::CapsuleType && colliderB->GetType() == Collider::SphereType)
	{
		return CollectContactPoints_Sphere_Capsule(colliderB->GetWorldSphere(), colliderA->GetWorldCapsule(), outContacts, contactOffset);
	}


	if (colliderA->GetType() == Collider::CapsuleType && colliderB->GetType() == Collider::CapsuleType)
	{
		return CollectContactPoints_Capsule_Capsule(colliderA->GetWorldCapsule(), colliderB->GetWorldCapsule(), outContacts, contactOffset);
	}
	return false;
	*/
}

void PhysWorld::ApplyIterations(std::vector<ContactManifold>& manifolds, float deltaTime)
{
	const int velocityIterations = 8; // 速度（跳ね返り・摩擦）の反復回数
	const int positionIterations = 3; // 位置（めり込み押し出し）の反復回数

	// 1. 速度のイテレーション（これを繰り返すとジェンガが安定する）
	for (int i = 0; i < velocityIterations; ++i) {
		for (auto& m : manifolds) {
			for (auto& point : m.gContactPoints) {
				// Rigidbodyの速度と角速度だけを更新する
				// ResolveCollisionの中から「座標更新」を抜いた処理を呼ぶ
				if (m.gRbA) {
					// Aが動的オブジェクトの場合
					m.gRbA->ResolveVelocity(m.gRbB, m.gNormal, point, deltaTime);
				}
				else if (m.gRbB) {
					// Aが静止物で、Bだけが動的オブジェクトの場合
					// Bから見て「A（静止物）」とぶつかった計算にするため、法線を反転して呼ぶ
					m.gRbB->ResolveVelocity(nullptr, m.gNormal, point, deltaTime);
				}
			}
		}
	}

	// 2. 位置のイテレーション（めり込みを解消する）
	for (int i = 0; i < positionIterations; ++i) {
		for (auto& m : manifolds) {
			// 座標を直接ズラす処理
			// ここで少しずつ押し出すことで、複数の衝突がある場合も矛盾しにくくなる
			ResolvePosition(m);
		}
	}
}

void PhysWorld::ResolvePosition(ContactManifold& m)
{

	// Rigidbodyの取得（付いていない場合は nullptr）
	Rigidbody* rbA = m.gRbA;
	Rigidbody* rbB = m.gRbB;

	// 質量の逆数を取得。Rigidbodyがない（静的）場合は 0.0f と見なす
	float invMassA = (rbA != nullptr) ? rbA->GetInverseMass() : 0.0f;
	float invMassB = (rbB != nullptr) ? rbB->GetInverseMass() : 0.0f;

	// 両方の InverseMass の合計
	float sumInvMass = invMassA + invMassB;

	// どちらも動かない物体（合計がゼロ）なら何もしない
	if (sumInvMass <= 0.0001f)
	{
		return;
	}

	// 押し出しの割合（AとBでどう分担するか）
	float ratioA = invMassA / sumInvMass;
	float ratioB = invMassB / sumInvMass;

	// めり込み量に対して、少し余裕（0.01fなど）を残して押し出す（ジッター対策）
	const float slop = 0.01f;
	const float percent = 0.8f; // 1.0にすると跳ねすぎることがあるので80%程度にする
	float correctionMagnitude = std::max(m.gPenetration - slop, 0.0f) * percent;

	// 各オブジェクトの押し出しベクトルを計算（重いほど動かない）
	Vector3 correctionA = m.gNormal * (correctionMagnitude * (invMassA / sumInvMass));
	Vector3 correctionB = m.gNormal * (correctionMagnitude * (invMassB / sumInvMass));

	if (m.gRbA)
	{
		// Aを法線方向に移動
		Vector3 posA = m.gRbA->GetOwner()->GetTransform()->GetLocalPosition();
		m.gRbA->GetOwner()->GetTransform()->SetLocalPosition(posA - correctionA);
		m.gRbA->GetOwner()->GetTransform()->ComputeWorldTransform();
	}

	if (m.gRbB)
	{
		// Bを法線の【逆】方向に移動
		Vector3 posB = m.gRbB->GetOwner()->GetTransform()->GetLocalPosition();
		m.gRbB->GetOwner()->GetTransform()->SetLocalPosition(posB + correctionB);
		m.gRbB->GetOwner()->GetTransform()->ComputeWorldTransform();
	}
	/*
	// 押し出す量（修正ベクトル）の計算
	// penetration（めり込み深さ）から slop を引いた分に percent を掛ける
	float correctionMagnitude = std::max(m.penetration - slop, 0.0f) * percent;
	Vector3 correction = m.normal * correctionMagnitude;

	if (m.rbA && m.rbB)
	{
		// --- 【動的 vs 動的】 ---
		// 質量に応じて押し出し量を按分する（軽い方がよく動く）
		float invMassA = m.rbA->GetInverseMass(); // (1.0f / mMass) を返す関数を想定
		float invMassB = m.rbB->GetInverseMass();
		float totalInvMass = invMassA + invMassB;

		Vector3 moveA = correction * (invMassA / totalInvMass);
		Vector3 moveB = correction * (invMassB / totalInvMass);

		// Aを法線方向に移動
		Vector3 posA = m.rbA->GetOwner()->GetTransform()->GetLocalPosition();
		m.rbA->GetOwner()->GetTransform()->SetLocalPosition(posA + moveA);
		m.rbA->GetOwner()->GetTransform()->ComputeWorldTransform();

		// Bを法線の【逆】方向に移動
		Vector3 posB = m.rbB->GetOwner()->GetTransform()->GetLocalPosition();
		m.rbB->GetOwner()->GetTransform()->SetLocalPosition(posB - moveB);
		m.rbB->GetOwner()->GetTransform()->ComputeWorldTransform();
	}
	else if (m.rbA && !m.rbB)
	{
		// --- 【動的 vs 静的】 ---
		// Aだけを100%押し出す
		Vector3 posA = m.rbA->GetOwner()->GetTransform()->GetLocalPosition();
		m.rbA->GetOwner()->GetTransform()->SetLocalPosition(posA + correction);
		m.rbA->GetOwner()->GetTransform()->ComputeWorldTransform();
	}
	else if (!m.rbA && m.rbB)
	{
		// --- 【動的 vs 静的】 ---
		// Aだけを100%押し出す
		Vector3 posB = m.rbB->GetOwner()->GetTransform()->GetLocalPosition();
		m.rbB->GetOwner()->GetTransform()->SetLocalPosition(posB + correction);
		m.rbB->GetOwner()->GetTransform()->ComputeWorldTransform();
	}
	*/
}

bool PhysWorld::CollectContactPoints_OBB_OBB(const OBB& a, const OBB& b, std::vector<ContactPoint>& outContacts, float contactOffset)
{
	Vector3 normal;
	float depth;
	Vector3 contactPoint;

	if (GetContactInfo_OBB(a, b, normal, depth, contactPoint))
	{
		outContacts.emplace_back(ContactPoint{ normal, depth, contactPoint });
		return true;
	}
	return false;
}

bool PhysWorld::GetContactInfo_OBB(const OBB& a, const OBB& b, Vector3& outNormal, float& outDepth,Vector3& contactPoint)
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
	// 法線はBからAへ向かっているので、
	// Aにとって最も深く刺さっている点は「-bestAxis方向」の頂点
	Vector3 supportA = GetSupportPoint(a, bestAxis);
	// Bにとって最も深く刺さっている点は「bestAxis方向」の頂点
	Vector3 supportB = GetSupportPoint(b, -1.0f * bestAxis);
	// 2つの頂点の中点を近似的な接触点とする
	contactPoint = (supportA + supportB) * 0.5f;
	return true;
}

bool PhysWorld::CollectContactPoints_Sphere_Sphere(const Sphere& a, const Sphere& b, std::vector<ContactPoint>& outContacts, float contactOffset)
{
	Vector3 diff = b.mCenter - a.mCenter;
	float dist = diff.Length();

	float radiusSum = a.mRadius + b.mRadius;
	float penetration = radiusSum - dist;

	if (penetration + contactOffset > 0.0f)
	{
		Vector3 normal = (dist > 0.0001f) ? diff / dist : Vector3::UnitX;
		// 接触点のワールド座標を計算
			// Sphere A の中心から法線方向に Sphere A の半径分進んだ点 (法線は A から B に向かう)
		Vector3 contactPoint = a.mCenter + normal * a.mRadius;

		// ContactPoint に position を追加
		outContacts.emplace_back(ContactPoint{ normal, penetration, contactPoint });
		return true;
	}
	return false;
}

bool PhysWorld::CollectContactPoints_Capsule_Capsule(const Capsule& a, const Capsule& b, std::vector<ContactPoint>& outContacts, float contactOffset)
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
		// 接触点のワールド座標
		// pA と pB の中間点（あるいは pA から a.mRadius 分進んだ点）
		Vector3 contactPoint = pa + normal * a.mRadius;

		// ContactPoint に position を追加
		outContacts.emplace_back(ContactPoint{ normal, penetration, contactPoint });
		return true;
	}
	return false;
}

bool PhysWorld::CollectContactPoints_OBB_Sphere(const OBB& a, const Sphere& b, std::vector<ContactPoint>& outContacts, float contactOffset)
{
	Vector3 closest = ClosestPointOnOBB(b.mCenter, a);
	Vector3 diff = b.mCenter - closest;
	float distSq = diff.LengthSq();
	float radius = b.mRadius;

	if (distSq <= (radius + contactOffset) * (radius + contactOffset))
	{
		float dist = std::sqrt(distSq);
		float penetration = radius - dist;

		// 押し出し応答が必要なのは、めり込みが発生している場合のみ
		if (penetration > 0.0001f) // 真にめり込んでいるかチェック
		{
			Vector3 normal = diff / dist;

			// 接触点のワールド座標を計算
			// Sphereの中心から法線方向にSphereの半径分戻った点を接触点とする
			Vector3 contactPoint = b.mCenter - normal * radius;

			// 接触点を追加
			outContacts.emplace_back(ContactPoint{ normal, penetration, contactPoint });
			return true;
		}
	}
	return false;
}

bool PhysWorld::CollectContactPoints_OBB_Capsule(const OBB& a, const Capsule& b, std::vector<ContactPoint>& outContacts, float contactOffset)
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

		// 接触点のワールド座標
		// Capsule の線分上の最近接点から法線と逆方向に半径分戻った点
		Vector3 contactPoint = bestPointOnSeg - normal * radius;

		// ContactPoint に position を追加
		outContacts.emplace_back(ContactPoint{ normal, penetration, contactPoint });
		return true;
	}
	return false;
}

bool PhysWorld::CollectContactPoints_Sphere_Capsule(const Sphere& a, const Capsule& b, std::vector<ContactPoint>& outContacts, float contactOffset)
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
		
		// 接触点のワールド座標
		// Sphere A の中心から法線と逆方向に Sphere A の半径分戻った点
		Vector3 contactPoint = a.mCenter - normal * a.mRadius;

		// ContactPoint に position を追加
		outContacts.emplace_back(ContactPoint{ normal, penetration, contactPoint });
		return true;
	}
	return false;
}

void PhysWorld::ClearAllCollider()
{
	mCollider.clear();
	mColliderXAxis.clear();
}


void PhysWorld::AddCollider(Collider* box)
{
	mCollider.push_back(box);
	mColliderXAxis.emplace_back(box);
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
