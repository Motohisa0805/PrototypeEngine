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
	int				mBoneIndex;
	//親のボーンの数値
	int				mParentIndex;

	//スケルトンのスキンメッシュ計算洋行列キャッシュ
    Matrix4			mGlobalInvBindPose;

public:
	BoneActor();
    // ボーンの名前を取得(ボーンの名前をアクターの名前に設定)
	void			SetBoneName(string name) { mName = name; }

	const int		GetBoneIndex() const { return mBoneIndex; }

	void			SetBoneIndex(int index) { mBoneIndex = index; }

	const int		GetParentIndex()const  { return mParentIndex; }

	void			SetParentIndex(int index) { mParentIndex = index; }


	void			SetGlobalInvBindPose(const Matrix4& mat) { mGlobalInvBindPose = mat; }
    const Matrix4&	GetGlobalInvBindPose() const { return mGlobalInvBindPose; }

	Matrix4			GetCurrentBoneMatrix() { return GetTransform()->GetWorldTransform(); }
};

