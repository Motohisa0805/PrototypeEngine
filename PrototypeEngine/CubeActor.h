#pragma once
#include "Actor.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"
#include "PushButtonMove.h"
#include "Mesh.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

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

