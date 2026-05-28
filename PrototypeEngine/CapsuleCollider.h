#pragma once
#include "Collider.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//カプセル型のコライダークラス
class CapsuleCollider : public Collider
{
private:

	Capsule			mObjectCapsule;
	
	Capsule			mWorldCapsule;
	
	bool			mShouldRotate;
public:
					CapsuleCollider(Entity* owner, int updateOrder = 100);
					~CapsuleCollider();

	void			OnUpdateWorldTransform() override;

	void			SetObjectCapsule(const Capsule& model) { mObjectCapsule = model; }

	OBB				GetWorldOBB() const override;

	Sphere			GetWorldSphere() const override { return Sphere(Vector3::Zero, 0.0f); }

	Capsule			GetWorldCapsule() const override { return mWorldCapsule; }

	void			SetShouldRotate(bool value) { mShouldRotate = value; }

	AABB			GetWorldAABBFromOBB() const override;

	ColliderType	GetType() override { return ColliderType::CapsuleType; }

	void			Serialize(json& j) const override;
	void			Deserialize(const json& j)override;

	void			DrawCustomGUI(const std::vector<PropertyInfo>& properties)override;

	Component*		Clone(Entity* newOwner) const override;
};