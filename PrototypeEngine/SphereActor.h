#pragma once
#include "Actor.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/
//前方宣言
class SphereCollider;

//球型のオブジェクト
//Unityのようにデフォルトで作成可能オブジェクト
class SphereActor : public ActorObject
{
private:
	//球のコライダー
	SphereCollider*			mSphere;
public:
							SphereActor();

	SphereCollider*			GetSphere() { return mSphere; }
};

