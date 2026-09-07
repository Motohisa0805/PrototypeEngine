#pragma once
#include "BoneTransform.h"
#include "Typedefs.h"
#include "Assimp.h"
#include "MatrixPalette.h"
#include "BoneActor.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/


class BoneTransform;
class BoneActor;

//スケルタルメッシュの骨格を管理するクラス
class SkeletonData
{
public:
	struct BoneInfo
	{
        char	   sName[SkeletonLayout::MAX_SKELETONBINBONE];
        uint32_t   sNodeHash;
        int		   sParentIndex;
        Matrix4	   sInverseBindPose;

		//アニメーションが適用されていない初期状態のローカルポーズ
        Vector3    sLocalPos   = Vector3::Zero;
        Quaternion sLocalRot   = Quaternion::Identity;
        Vector3    sLocalScale = Vector3::UnitXYZ;
	};
	//スケルトンのタイプのタグ
	enum SkeletonType
	{
		Humanoid,
		Generic
	};
protected:
	// スケルトンがロードされると自動的に呼び出され、
	// 各ボーンのグローバルインバインドポーズを計算。
	void									ComputeGlobalInvBindPose();
private:
    vector<BoneInfo>						mBones;
	//文字列とint型の連想配列
	std::unordered_map<string, int>			mBoneNameToIndex;
	// スケルトンのタイプ
	// 現在は未使用
	//SkeletonType							mSkeletonType;

	filesystem::path						mSkeletonFilePath;

public:
	~SkeletonData();

	vector<BoneInfo>						GetBones() const { return mBones; }

	filesystem::path						GetSkeletonFilePath() { return mSkeletonFilePath; }
	//すべてのファイル形式から読み込み
	bool									Load(const string& fileName);
	//バイナリデータから読み込み
	bool									LoadFromSkeletonBin(const string& fileName);

	//FBXファイルから読み込み
	bool									LoadFromFBX(const string& fileName);
	//ボーンの親を設定
	void									SetParentBones(aiNode* node, int parentIndex,const aiMatrix4x4& parentTransform = aiMatrix4x4());

	bool EndsWith(const std::string& str, const std::string& suffix)
	{
		if (str.size() < suffix.size())
		{
			return false;
		}
		return str.substr(str.size() - suffix.size()) == suffix;
	}

	//ボーンの連想配列のGetter
	const std::unordered_map<string, int>&	GetBoneNameToIndex() const { return mBoneNameToIndex; }
	//ActorObjectの親を設定
	void									SetParentActor(ActorObject* parent);


	static ActorObject*						FindActorByName(ActorObject* current, const string& name);
};