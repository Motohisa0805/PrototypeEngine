#pragma once
#include "Actor.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "Mesh.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//3Dモデルを読み込むクラス
//スケルトン無しの3Dモデルを
//読み込みだけのクラス
//ActorObjectを継承しているので1オブジェクトとしてカウント
class MeshActor : public ActorObject
{
private:
	MeshRenderer*				mMeshRenderer;

	BoxCollider*				mBoxCollider;

	SphereCollider*				mSphereCollider;

	CapsuleCollider*			mCapsuleCollider;

public:
	//コンストラクタ
							MeshActor();
							~MeshActor();

	void					Load(string filePath);

	void					SetColliderMode(bool active);

	void					SetStaticMode(bool active);

	void					AddBoxCollider();

	void					AddSphereCollider();

	void					AddCapsuleCollider();

	MeshRenderer*			GetMeshRenderer() { return mMeshRenderer; }

	BoxCollider*			GetBoxCollider() { return mBoxCollider; }

	SphereCollider*			GetSphereCollider() { return mSphereCollider; }

	CapsuleCollider*		GetCapsuleCollider() { return mCapsuleCollider; }

	//MeshRendererからAABBを取得処理を省略した関数
	const AABB				GetBoxAABB();
	//MeshRendererからOBBを取得処理を省略した関数
	const OBB				GetBoxOBB();
};