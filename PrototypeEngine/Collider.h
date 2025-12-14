#pragma once
#include "Component.h"
#include "Collision.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

class ActorObject;

//コライダーの基底クラス
//コライダーに必要な情報を持っています。
// UnityのColliderとTriggerの機能を持っています。
class Collider : public Component
{
protected:
	// ワールド空間OBB
	OBB						mWorldOBB;	

	AABB					mWorldAABB;
	//オブジェクト同士の当たり判定をするかしないか
	//UnityのColliderかTriggerかを切り替えるフラグ
	bool					mIsCollider = true;

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

	virtual Sphere			GetWorldSphere() const = 0;
	virtual Capsule			GetWorldCapsule() const = 0;
	virtual AABB			GetWorldAABBFromOBB() const { return mWorldAABB; }

	virtual OBB				GetWorldOBB() const = 0;

	bool					IsCollider() { return mIsCollider; }
	void					SetCollider(bool active) { mIsCollider = active; }

	float					GetContactOffset() { return mContactOffset; }

	void					Serialize(json& j) const override;
	void					Deserialize(const json& j)override;

	void					DrawCustomGUI(const std::vector<PropertyInfo>& properties)override;
};

