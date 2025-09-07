#pragma once
#include "BoneTransform.h"
#include "Assimp.h"
#include "SDL3.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

namespace AnimationLayout
{
	constexpr float DEFAULTTICKSPERSECOND = 25.0f;
}

//書籍元を改造したファイル
struct AnimationBinHeader 
{
	uint32_t	version = 1;
	float		duration = 0;
	uint32_t	numFrames = 0;
	uint32_t	numBones = 0;
};

struct AnimationBinTransform 
{
	Vector3		position = Vector3();
	Quaternion	rotation = Quaternion();
	Vector3		scale = Vector3();
};

//前方宣言
class Skeleton;

//アニメーション1つの情報を持つクラス
//アニメーションのフレーム数、持続時間、各ボーンの変形情報を持つ
//アニメーションの読み込みはAssimpを使用
class Animation
{
private:
	//bool									LoadFromJSON(const string& fileName);

	bool									LoadFromFBX(const string& fileName);

	size_t									FindTranslation(float AnimationTime, const aiNodeAnim* pNodeAnim);

	void									CalcInterpolatedTranslation(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

	size_t									FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);

	void									CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

	size_t									FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

	void									CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	// アニメーションのための骨の数
	size_t									mNumBones;
	// アニメーションのフレーム数
	size_t									mNumFrames;
	// アニメーションの持続時間（秒）
	float									mDuration;
	// アニメーションにおける各フレームの持続時間
	float									mFrameDuration;
	// トラック上の各フレームに対する情報を変換。
	// 外側のベクトルの各インデックスは骨であり、
	// 内側のベクトルはフレームです。
	vector<vector<BoneTransform>>			mTracks;

	vector<Vector3*>						mRootPosition;

	Skeleton* mSkeleton;
	//アニメーションをループさせるためのフラグ
	bool									isLoop;
	//アニメーションが再生終了したかどうか
	bool									isAnimationEnd;
	//ルートモーションのフラグ
	bool									isRootMotion;
	bool									isRootMotionX;
	bool									isRootMotionY;
	bool									isRootMotionZ;
	//ルートモーションのオフセット
	float									mRootMotionX;
	float									mRootMotionY;
	float									mRootMotionZ;

	bool									isReLoad;

	string									mFileName;

	string									mAnimationName;

	vector<Vector3>							mRootPositionOffset;

public:
											Animation(Skeleton* skeleton);

	bool									Load(const string& fileName);

	bool									ReLoad();

	bool									LoadFromBinary(const std::string& filePath);

	bool									SaveToBinary(const std::string& filePath);

	void									Update();

	size_t									GetNumBones() const { return mNumBones; }
	
	size_t									GetNumFrames() const { return mNumFrames; }
	
	float									GetDuration() const { return mDuration; }
	
	float									GetFrameDuration() const { return mFrameDuration; }

	void									SetSkeleton(Skeleton* skeleton) { mSkeleton = skeleton; }

	bool									IsLoop() const { return isLoop; }

	void									SetLoop(bool loop) { isLoop = loop; }

	bool									IsAnimationEnd() const { return isAnimationEnd; }

	void									SetIsAnimationEnd(bool animationEnd) { isAnimationEnd = animationEnd; }

	bool									IsRootMotion()const { return isRootMotion; }

	void									SetRootMotion(bool active) 
	{
		isRootMotion = active; 
		isReLoad = true;
	}
	void									SetIsRootMotion(bool x,bool y,bool z)
	{
		isRootMotionX = x;
		isRootMotionY = y;
		isRootMotionZ = z;
		isReLoad = true;
	}

	float									GetRootMotionX() { return mRootMotionX; }
	float									GetRootMotionY() { return mRootMotionY; }
	float									GetRootMotionZ() { return mRootMotionZ; }

	void									SetRootMotionX(float num) 
	{
		mRootMotionX = num; 
		isReLoad = true;
	}
	void									SetRootMotionY(float num) 
	{
		mRootMotionY = num;
		isReLoad = true;
	}
	void									SetRootMotionZ(float num) 
	{
		mRootMotionZ = num; 
		isReLoad = true;
	}
	void									SetRootMotionPosition(Vector3 pos)
	{
		mRootMotionX = pos.x;
		mRootMotionY = pos.y;
		mRootMotionZ = pos.z;
		isReLoad = true;
	}

	// 指定されたアニメーションの時間における各ボーンのグローバル（現在の）ポーズ行列を提供されたベクターに充填。
	// 時間は0.0f以上でmDuration以下であること。
	void									GetGlobalPoseAtTime(vector<Matrix4>& outPoses, const Skeleton* inSkeleton, float inTime) const;
};