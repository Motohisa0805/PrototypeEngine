#pragma once
#include "Component.h"
#include "Collision.h"
#include "Actor.h"
#include "BaseScene.h"
#include "PhysWorld.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//コライダーの基底クラス
//コライダーに必要な情報を持っています。
// UnityのColliderとTriggerの機能を持っています。
class Collider : public Component
{
protected:
	// ワールド空間OBB
	OBB						mWorldOBB;	

	AABB					mWorldBox;
	//オブジェクト同士の当たり判定をするかしないか
	//UnityのColliderかTriggerかを切り替えるフラグ
	bool					mIsCollider = true;
	//オブジェクトを動かさないか動かすかを決める
	//Rigidbodyのような機能がないため追加
	bool					mStaticObject = true;

	float					mContactOffset;
public:
							Collider(class ActorObject* owner, int updateOrder = 100);
							~Collider();

	enum ColliderType
	{
		BoxType,
		SphereType,
		CapsuleType
	};
	virtual ColliderType	GetType() = 0;

	virtual const Sphere&	GetWorldSphere() const = 0;
	virtual const Capsule&	GetWorldCapsule() const = 0;
	virtual AABB			GetWorldAABBFromOBB() const { return mWorldBox; }

	virtual OBB				GetWorldOBB() const = 0;

	bool					IsCollider() { return mIsCollider; }
	void					SetCollider(bool active) { mIsCollider = active; }

	bool					IsStaticObject() { return mStaticObject; }
	void					SetStaticObject(bool active) { mStaticObject = active; }

	float					GetContactOffset() { return mContactOffset; }

};

