#pragma once
#include "MeshActor.h"
#include "SphereCollider.h"
#include "BoxCollider.h"

//弾のActorクラス
class BulletActor : public ActorObject
{
private:
	//弾のメッシュ
	MeshActor*				mBullet;
	// 弾の力の方向
	Vector3					mPowerDirection;
	
	SphereCollider*			mSphereCollider;
	// ボックスコライダー
	BoxCollider*			mBoxCollider;
	// 弾の破壊までの時間
	float					mDestroyCount;
	// 弾の力
	float					mPower;
public:
							BulletActor(Vector3 powerDir,Vector3 position);
							~BulletActor();

	void					UpdateActor(float deltaTime) override;

	void					OnCollisionEnter(class ActorObject* target) override;
};

