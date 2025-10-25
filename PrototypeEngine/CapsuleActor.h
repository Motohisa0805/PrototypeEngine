#pragma once
#include "Actor.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

class CapsuleCollider;

//カプセル型のオブジェクト
//Unityのようにデフォルトで作成可能オブジェクト
class CapsuleActor : public ActorObject
{
private:
	//カプセルコライダー
	CapsuleCollider*		mCapsule;
public:
							CapsuleActor();

	CapsuleCollider*		GetCapsule() { return mCapsule; }
};