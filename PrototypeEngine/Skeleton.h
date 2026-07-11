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
class Skeleton
{
public:
	//スケルトンのバイナリデータ構造体
	struct SkeletonBinBone
	{
		// ボーン名（固定長）
		char		name[SkeletonLayout::MAX_SKELETONBINBONE];
		// 短縮版ボーン名
		char		shortName[SkeletonLayout::MAX_SKELETONBINBONE];
		// 親ボーンインデックス（-1 なら root）
		int32_t		parentIndex;      
		// バインドポーズの位置
		Vector3		position;         
		// バインドポーズの回転
		Quaternion	rotation;      
		// バインドポーズのスケール（オプション）
		Vector3		scale;            
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
    // 各骨の情報を格納するアクター
	vector<BoneActor*>						mBoneActors;
	//計算用のオフセット変数
	vector<aiMatrix4x4>						mOffsetMatrix;
	//文字列とint型の連想配列
	std::unordered_map<string, int>			mBoneNameToIndex;
	//文字列とint型の連想配列
	std::unordered_map<string, int>			mBoneTransform;
	// スケルトンのタイプ
	// 現在は未使用
	//SkeletonType							mSkeletonType;
public:
	~Skeleton();
	//すべてのファイル形式から読み込み
	bool									Load(const string& fileName);
	//バイナリデータから読み込み
	bool									LoadFromSkeletonBin(const string& fileName);

	//FBXファイルから読み込み
	bool									LoadFromFBX(const string& fileName);
	//ボーンの親を設定
	void									SetParentBones(aiNode* node, int parentIndex);

	bool EndsWith(const std::string& str, const std::string& suffix)
	{
		if (str.size() < suffix.size())
		{
			return false;
		}
		return str.substr(str.size() - suffix.size()) == suffix;
	}

	// ボーン数のGetter
	size_t									GetNumBones() const { return mBoneActors.size(); }
	//ボーンのGetter Ver.1
	const BoneActor&						GetBone(size_t idx) const { return *mBoneActors[idx]; }
	//ボーンGetter Ver.2
	const vector<BoneActor*>&				GetBones() const { return mBoneActors; }
	//ボーンオブジェクトのGetter
	vector<BoneActor*>						GetBoneActor() const { return mBoneActors; }

	// ボーンのグローバルバインドポーズのGetter
    Matrix4 GetGlobalInvBindPose(size_t idx) const
    {
        return mBoneActors[idx]->GetGlobalInvBindPose();
    }
    // アニメーション適用後のボーンの行列変数のGetter
    Matrix4 GetGlobalCurrentPose(size_t idx) const
    {
        return mBoneActors[idx]->GetTransform()->GetWorldTransform();
    }

	//ボーンの連想配列のGetter
	const std::unordered_map<string, int>&	GetBoneNameToIndex() const { return mBoneNameToIndex; }
	//指定したボーンにオブジェクトを子オブジェクトとして設定
	void									AddBoneChildActor(string boneName,class ActorObject* actor);
	//ActorObjectの親を設定
	void									SetParentActor(ActorObject* parent);
};