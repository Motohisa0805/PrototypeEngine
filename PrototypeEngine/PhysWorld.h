#pragma once
#include "StandardLibrary.h"
#include "Math.h"
#include "Collision.h"
#include "Rigidbody.h"
#include "Physics.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class Collider;
class ActorObject;
class BaseScene;

struct ContactManifold {
	Rigidbody*		gRbA;
	Rigidbody*		gRbB;
	Vector3			gNormal;
	float			gPenetration;
	vector<Vector3> gContactPoints;
};

//すべてのオブジェクトの衝突判定を管理しているクラス
//Unityの当たり判定の衝突判定部分みたいな機能
class PhysWorld
{
public:

	// 衝突結果に関する情報をまとめている(※Ray関係の構造体)
	struct CollisionInfo
	{
		// 衝突点
		Vector3				mPoint;
		// 衝突時の座標
		Vector3				mNormal;
		float				mT;
		// 衝突クラス
		Collider*			mCollider = nullptr;
		// コンポーネントの所有Actor
		ActorObject*		mActor = nullptr;
	};

	struct ContactPoint
	{
		Vector3 mNormal;        // 接触法線
		float mPenetration;     // めり込み深さ
		Vector3 mPosition;	   // 接触点のワールド座標
	};

private:

	vector<Collider*>									mCollider;

	vector<Collider*>									mColliderXAxis;

	// 前のフレームの衝突ペア（Actor同士）を管理するセット。これを使って、衝突開始/継続/終了イベントを判定する。
	std::set<std::pair<ActorObject*, ActorObject*>>		mPrevHitPairs;
	// 現在の衝突ペア（Actor同士）を管理するセット。これを使って、衝突開始/継続/終了イベントを判定する。
	std::set<std::pair<ActorObject*, ActorObject*>>		mCurrentHitPairs;

	vector<Vector3>										GetOBBIncidentFace(const class OBB& obb,const Vector3& normal);

	// ポリゴンを平面でカットする（クリッピング）
	void												ClipPolygonAgainstPlane(const std::vector<Vector3>& inPoly, const Vector3& planeNormal, const Vector3& planePoint, std::vector<Vector3>& outPoly);

public:

														PhysWorld();

	// 線分をボックスに対して判定します。
	// ボックスに衝突する場合は真を返します。
	bool												RayCast(const LineSegment& l, CollisionInfo& outColl,int tag = -1);

	std::vector<CollisionInfo>							RayCastAll(const LineSegment& l);

	//XYZのSweeppruneを使用した衝突判定
	void												SweepAndPruneXYZ();
	//各コライダー同士の精密判定をまとめた関数
	bool												IsOnCollision(Collider* colliderA, Collider* colliderB);
	//各コライダー同士の押し出し処理をまとめた関数
	bool												IsCollectContactPoints(class Collider* colliderA, class Collider* colliderB, std::vector<ContactPoint>& outContacts, float contactOffset);
	//衝突の解決を一定数繰り返す関数
	void												ApplyIterations(std::vector<ContactManifold>& manifolds, float deltaTime);
	//衝突の解決を1回行う関数
	void												OneResolvePosition(ContactManifold& m);
	//衝突の解決をすべてのマニホールドに対して行う関数
	void												ResolvePositions(std::vector<ContactManifold>& manifolds,int index);

	//OBB vs OBBの押し出し処理
	bool												CollectContactPoints_OBB_OBB(const OBB& a, const OBB& b, std::vector<ContactPoint>& outContacts, float contactOffset);
	//OBB vs OBBの押し出し処理に使う関数
	bool												GetContactInfo_OBB(const OBB& a, const OBB& b, Vector3& outNormal, float& outDepth,Vector3& contactPoint);
	//Sphere vs Sphereの押し出し処理
	bool												CollectContactPoints_Sphere_Sphere(const Sphere& a, const Sphere& b, std::vector<ContactPoint>& outContacts, float contactOffset);
	//Capsule vs Capsuleの押し出し処理
	bool												CollectContactPoints_Capsule_Capsule(const Capsule& a, const Capsule& b, std::vector<ContactPoint>& outContacts, float contactOffset);
	//OBB vs Sphereの押し出し処理
	bool												CollectContactPoints_OBB_Sphere(const OBB& a, const Sphere& b, std::vector<ContactPoint>& outContacts, float contactOffset);
	//OBB vs Capsuleの押し出し処理
	bool												CollectContactPoints_OBB_Capsule(const OBB& a, const Capsule& b, std::vector<ContactPoint>& outContacts, float contactOffset);
	//Sphere vs Capsuleの押し出し処理
	bool												CollectContactPoints_Sphere_Capsule(const Sphere& a, const Capsule& b, std::vector<ContactPoint>& outContacts, float contactOffset);

	void												ClearAllCollider();
	// 世界からボックスコンポーネントを追加/削除する
	void												AddCollider(Collider* box);
	void												RemoveCollider(Collider* box);
};

inline Vector3 GetSupportPoint(const OBB& obb, const Vector3& dir)
{
	Vector3 result = obb.mCenter;
	Vector3 axes[3] = {
		Vector3::Transform(Vector3::UnitX, obb.mRotation),
		Vector3::Transform(Vector3::UnitY, obb.mRotation),
		Vector3::Transform(Vector3::UnitZ, obb.mRotation)
	};
	float extents[3] = { obb.mExtents.x, obb.mExtents.y, obb.mExtents.z };

	for (int i = 0; i < 3; ++i)
	{
		// 軸がdirと同じ方向を向いているか判定し、その方向に符号を掛ける
		float sign = Math::Sign(Vector3::Dot(dir, axes[i]));
		result += axes[i] * (sign * extents[i]);
	}
	return result;
}