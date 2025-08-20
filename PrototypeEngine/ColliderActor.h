#pragma once
#include "Collider.h"
#include "Rigidbody.h"
#include "Transform.h"

//当たり判定、物理挙動のクラスをまとめたActorObjectクラス
//現在は未使用
class ColliderActor : public Transform
{
public:
	ColliderActor();
	virtual ~ColliderActor();
	//当たった時に呼び出される関数
	virtual void				OnCollisionEnter(class ActorObject* target){}
	//当たっている時に呼び出される関数
	virtual void				OnCollisionStay(class ActorObject* target) {}
	//当たり終わった時に呼び出される関数
	virtual void				OnCollisionExit(class ActorObject* target) {}

	virtual class Rigidbody*	GetRigidbody() { return mRigidbody; }

	virtual class Collider*		GetCollider() { return mCollider; }
protected:
	//物理処理管理クラス
	class Rigidbody*			mRigidbody;
	//当たり判定クラス
	class Collider*				mCollider;
};

