#pragma once
#include "Collider.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//球のColliderを追加するコンポーネント
class SphereCollider : public Collider
{
private:

	Sphere			mObjectSphere;
	
	Sphere			mWorldSphere;
	
	bool			mShouldRotate;
public:
					SphereCollider(Entity* owner, int updateOrder = 100);
					~SphereCollider();
	void			OnUpdateWorldTransform() override;

	void			SetObjectSphere(const Sphere& model) { mObjectSphere = model; }
	
	OBB				GetWorldOBB() const override;

	Sphere			GetWorldSphere() const override { return mWorldSphere; }

	Capsule			GetWorldCapsule() const override { return Capsule(Vector3::Zero, Vector3::Zero, 0.0f); }

	void			SetShouldRotate(bool value) { mShouldRotate = value; }

	AABB			GetWorldAABBFromOBB() const override;

	ColliderType	GetType() override { return ColliderType::SphereType; }

	void			Serialize(json& j) const override;
	void			Deserialize(const json& j)override;

	void			DrawCustomGUI(const std::vector<PropertyInfo>& properties)override;

	Component*		Clone(ActorObject* newOwner) const override;
};