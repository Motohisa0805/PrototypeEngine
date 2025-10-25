#pragma once
#include "Actor.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

class BoxCollider;

//箱型のオブジェクト
//Unityのようにデフォルトで作成可能オブジェクト
class CubeActor : public ActorObject
{
private:
	//AABBのコライダー
	BoxCollider*	mBox;
public:
					CubeActor();

	BoxCollider*	GetBoxs() { return mBox; }

};

