#pragma once
#include "Actor.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"
#include "Mesh.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//平面型のオブジェクト
//Unityのようにデフォルトで作成可能オブジェクト
class PlaneActor : public ActorObject
{
private:
	//AABBのコライダー
	vector<class BoxCollider*>	mBoxs;

	class MeshRenderer*			mMeshComp;
public:
								PlaneActor();
	vector<class BoxCollider*>	GetBoxs() { return mBoxs; }
};