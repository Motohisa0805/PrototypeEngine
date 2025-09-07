#pragma once
#include "Actor.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//スケルタルメッシュのボーン1つ1つの情報を管理するBoneActor
//主に子オブジェクトの位置管理を行っています。
class BoneActor : public ActorObject
{
private:
	string			boneName;

	int				boneIndex;

	Matrix4			mBoneMatrix;

	ActorObject*	parentActor;
public:
	void			ComputeWorldTransform(const Matrix4* parentMatrix)override;

	void			SetBoneName(string name) { boneName = name; }

	void			SetBoneIndex(int index) { boneIndex = index; }

	void			SetBoneMatrix(Matrix4 mat) { mBoneMatrix = mat; }

	void			SetParentActor(ActorObject* actor) { parentActor = actor; }
};

