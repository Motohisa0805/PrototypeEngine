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
	//親のボーンの数値
	int				mParentIndex;
public:
	BoneActor();
	void			ComputeWorldTransform()override;

	void			SetBoneName(string name) { boneName = name; mName = name; }

	void			SetBoneIndex(int index) { boneIndex = index; }

	void			SetBoneMatrix(Matrix4 mat) { mBoneMatrix = mat; }

	const int		GetParentIndex()const  { return mParentIndex; }

	void			SetParentIndex(int index) { mParentIndex = index; }
};

