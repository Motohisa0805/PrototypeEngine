#pragma once
#include "Collider.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//ボックスのColliderを追加するコンポーネント
// UnityのBoxColliderに近いクラス
// OBBを使用しているので、回転可能なコライダー
class BoxCollider : public Collider
{
private:
	//ローカル空間AABB
	AABB			mObjectBox;

	// ローカル空間OBB
	OBB				mObjectOBB;
	
	bool			mShouldRotate;
public:
					BoxCollider(class ActorObject* owner, int updateOrder = 100);
					~BoxCollider();

	void			OnUpdateWorldTransform() override;

	void			SetObjectBox(const AABB& model) { mObjectBox = model; }

	void			SetObjectOBB(const OBB& model) { mObjectOBB = model; }

	//OBBのGetter
	OBB				GetWorldOBB() const override { return mWorldOBB; }

	OBB				GetObjectOBB() const { return mObjectOBB; }
	//OBBからAABBへ変換
	AABB			GetWorldAABBFromOBB() const override;
	//Null
	const Sphere&	GetWorldSphere() const override { return Sphere(Vector3::Zero, 0.0f); }
	//Null
	const Capsule&	GetWorldCapsule() const override { return Capsule(Vector3::Zero, Vector3::Zero, 0.0f); }
	//回転フラグ
	void			SetShouldRotate(bool value) { mShouldRotate = value; }
	//コライダーのタグ
	ColliderType	GetType() override { return ColliderType::BoxType; }
};