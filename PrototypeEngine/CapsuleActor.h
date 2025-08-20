#pragma once
#include "Actor.h"
#include "MeshRenderer.h"
#include "CapsuleCollider.h"
#include "Mesh.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//カプセル型のオブジェクト
//Unityのようにデフォルトで作成可能オブジェクト
class CapsuleActor : public ActorObject
{
private:
	//カプセルコライダー
	class CapsuleCollider*	mCapsule;
public:
							CapsuleActor();

	class CapsuleCollider*	GetCapsule() { return mCapsule; }
};