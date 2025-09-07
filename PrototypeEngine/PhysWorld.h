#pragma once
#include "StandardLibrary.h"
#include "Math.h"
#include "Collision.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class Collider;
class ActorObject;
class BaseScene;

//書籍部分を改造したファイル
//すべてのオブジェクトの衝突判定を管理しているクラス
//Unityの当たり判定の衝突判定部分みたいな機能
class PhysWorld
{
public:
	enum Axis
	{
		X,
		Y,
		Z
	};

	// 衝突結果に関する情報をまとめている
	struct CollisionInfo
	{
		// 衝突点
		Vector3				mPoint;
		// 衝突時の座標
		Vector3				mNormal;
		float				mT;
		// 衝突クラス
		Collider*			mCollider;
		// コンポーネントの所有Actor
		ActorObject*		mActor;
	};

	struct ContactPoint
	{
		Vector3 normal;        // 接触法線
		float penetration;     // めり込み深さ
	};

private:
	class BaseScene*									mGame;

	vector<Collider*>									mCollider;

	vector<Collider*>									mColliderXAxis;

	vector<Collider*>									mColliderYAxis;

	vector<Collider*>									mColliderZAxis;

	std::set<std::pair<Collider*, Collider*>>			mHitColliderXAxis;

	std::set<std::pair<Collider*, Collider*>>			mHitColliderYAxis;

	std::set<std::pair<Collider*, Collider*>>			mHitColliderZAxis;

	vector<ActorObject*>							mDeadActors;

	std::set<std::pair<ActorObject*, ActorObject*>>		mPrevHitPairs;

	std::set<std::pair<ActorObject*, ActorObject*>>		mCurrentHitPairs;

public:

														PhysWorld(BaseScene* game);

	// 線分をボックスに対して判定します。
	// ボックスに衝突する場合は真を返します。
	bool												RayCast(const LineSegment& l, CollisionInfo& outColl,int tag = -1);

	std::vector<CollisionInfo>							RayCastAll(const LineSegment& l);

	//XYZのSweeppruneを使用した衝突判定
	void												SweepAndPruneXYZ();
	//押し出し処理の呼び出し元
	void												FixCollisions(Collider* dynamicCollider, Collider* staticCollider);
	//各コライダー同士の精密判定をまとめた関数
	bool												IsOnCollision(Collider* colliderA, Collider* colliderB);
	//各コライダー同士の押し出し処理をまとめた関数
	void												IsCollectContactPoints(class Collider* colliderA, class Collider* colliderB, std::vector<ContactPoint>& outContacts, float contactOffset);

	//OBB vs OBBの押し出し処理
	void												CollectContactPoints_OBB_OBB(const OBB& a, const OBB& b, std::vector<ContactPoint>& outContacts, float contactOffset);
	//OBB vs OBBの押し出し処理に使う関数
	bool												GetContactInfo_OBB(const OBB& a, const OBB& b, Vector3& outNormal, float& outDepth);
	//Sphere vs Sphereの押し出し処理
	void												CollectContactPoints_Sphere_Sphere(const Sphere& a, const Sphere& b, std::vector<ContactPoint>& outContacts, float contactOffset);
	//Capsule vs Capsuleの押し出し処理
	void												CollectContactPoints_Capsule_Capsule(const Capsule& a, const Capsule& b, std::vector<ContactPoint>& outContacts, float contactOffset);
	//OBB vs Sphereの押し出し処理
	void												CollectContactPoints_OBB_Sphere(const OBB& a, const Sphere& b, std::vector<ContactPoint>& outContacts, float contactOffset);
	//OBB vs Capsuleの押し出し処理
	void												CollectContactPoints_OBB_Capsule(const OBB& a, const Capsule& b, std::vector<ContactPoint>& outContacts, float contactOffset);
	//Sphere vs Capsuleの押し出し処理
	void												CollectContactPoints_Sphere_Capsule(const Sphere& a, const Capsule& b, std::vector<ContactPoint>& outContacts, float contactOffset);

	// 世界からボックスコンポーネントを追加/削除する
	void												AddCollider(Collider* box);
	void												RemoveCollider(Collider* box);
};