#pragma once
#include "BoneTransform.h"
#include "BoneActor.h"
#include "Assimp.h"
#include "MatrixPalette.h"
#include <GL/glew.h>
#include "Actor.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//書籍元を改造したファイル

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
//スケルタルメッシュの骨格を管理するクラス
class Skeleton
{
public:
	//スケルトンのタイプのタグ
	enum SkeletonType
	{
		Humanoid,
		Generic
	};
	// 骨格の各骨の定義
	struct Bone
	{
		BoneTransform	mLocalBindPose;
		string			mName;
		string			mShortName;
		int				mParent;
	};
protected:
	// スケルトンがロードされると自動的に呼び出され、
	// 各ボーンのグローバルインバインドポーズを計算。
	void									ComputeGlobalInvBindPose();
private:
	// 骨格の骨
	vector<Bone>							mBones;
	vector<BoneActor*>						mBoneActors;
	//計算用のオフセット変数
	vector<aiMatrix4x4>						mOffsetMatrix;
	// 各Boneに対するグローバル逆束縛ポーズ
	vector<Matrix4>							mGlobalInvBindPoses;
	//スケルトンのボーンのアニメーション適用後の座標を持つマトリックス
	vector<Matrix4>							mGlobalCurrentPoses;
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
	//ボーンの名前を短縮して出力
	string									ConvertSimpleBoneName(string boneName);

	bool EndsWith(const std::string& str, const std::string& suffix)
	{
		if (str.size() < suffix.size())
		{
			return false;
		}
		return str.substr(str.size() - suffix.size()) == suffix;
	}

	// ボーン数のGetter
	size_t									GetNumBones() const { return mBones.size(); }
	//ボーンのGetter Ver.1
	const Bone&								GetBone(size_t idx) const { return mBones[idx]; }
	//ボーンGetter Ver.2
	const vector<Bone>&						GetBones() const { return mBones; }
	//ボーンオブジェクトのGetter
	vector<BoneActor*>						GetBoneActor() const { return mBoneActors; }
	//ボーンのグローバルバインドポーズのGetter
	const vector<Matrix4>&					GetGlobalInvBindPoses() const { return mGlobalInvBindPoses; }
	//アニメーション適用後のボーンの行列変数のGetter
	const vector<Matrix4>&					GetGlobalCurrentPoses() const { return mGlobalCurrentPoses; }
	//アニメーション適用語の各ボーンの行列をセットするSetter
	void									SetGlobalCurrentPoses(vector<Matrix4>& poses) { mGlobalCurrentPoses = poses; }
	//ボーンの連想配列のGetter
	const std::unordered_map<string, int>&	GetBoneNameToIndex() const { return mBoneNameToIndex; }
	//ボーン名からそのボーンのマトリックスの取得
	Matrix4									GetBonePosition(string boneName);
	//指定したボーンにオブジェクトを子オブジェクトとして設定
	void									AddBoneChildActor(string boneName,class ActorObject* actor);
	//ActorObjectの親を設定
	void									SetParentActor(ActorObject* parent);
};