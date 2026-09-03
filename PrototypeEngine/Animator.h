#pragma once
#include "MatrixPalette.h"
#include "Animation.h"
#include "Skeleton.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//複数個あるアニメーションを管理するクラス
//Unityの「Animator」を意識して設計
//アニメーションの再生、ブレンドなどを行う
class Animator : public Component
{
private:

	vector<ActorObject*>			mBones;

	//アニメーションを配列で持ってる変数
	vector<Animation*>				mAnimations;

	SkeletonData*					mSkeleton;

	Animation*						mAnimation;

	Animation*						mBlendAnimation;
	//アニメーションの倍率
	float							mAnimPlayRate;
	//現在再生中のアニメーションの時間
	float							mAnimTime;
	//ブレンドアニメーションの時間
	float							mBlendAnimTime;
	// 現在のブレンド経過時間
	float							mBlendElapsed;
	//アニメーションのブレンドを行うためのフラグ
	bool							mBlending;
public:
    Animator(Entity* owner);
	~Animator();
	//アニメーション読み込み処理
	bool							Load(const string& fileName,bool animLoop = 0,bool rootMotion = 0);
	// アニメーションの更新処理
	void							Update(float deltaTime)override;
	// アニメーションを再生します。
	// アニメーションの長さを返します。
	float							PlayAnimation(Animation* anim);
	// ブレンドアニメーションを再生します。
	float							PlayBlendAnimation(Animation* anim);
    /*
	//スケルトンとアニメーションを使ってスキニング行列（palette）とボーンの可視化更新を行う
	void							ComputeMatrixPalette();
	//ブレンドアニメーションのスキニング行列（palette）とボーンの可視化更新を行う
	void							BlendComputeMatrixPalette();
	*/
	//アニメーションの配列のGetter
	vector<Animation*>				GetAnimations() { return mAnimations; }
    void							AddAnimation(Animation* anim);
	//スケルトンのGetter
	SkeletonData*					GetSkeleton() { return mSkeleton; }
	//スケルトンのSetter
	void							SetSkeleton(SkeletonData* skeleton);
    void							LoadSkeletonData(const string& fileName,ActorObject* rootBone);
	//現在再生中のアニメーションのGetter
	Animation*						GetAnimation() { return mAnimation; }
	//ブレンドアニメーションのGetter
	Animation*						GetBlendAnimation() { return mBlendAnimation; }
	//アニメーションの倍率
	float							GetAnimPlayRate() { return mAnimPlayRate; }
	//アニメーションの倍率をセットする関数
	void							SetAnimPlayRate(float playRate) { mAnimPlayRate = playRate; }
	//現在再生中のアニメーションの時間
	float							GetAnimTime() { return mAnimTime; }
	//ブレンドアニメーションの時間
	float							GetBlendAnimTime() { return mBlendAnimTime; }
	// 現在のブレンド経過時間
	float							GetBlendElapsed() { return mBlendElapsed; }
	//ブレンドアニメーションの時間をセットする関数
	void							SetBlendElapsed(float elapsed) { mBlendElapsed = elapsed; }
	//現在のアニメーションの時間を正規化した値を出力する関数
	float							GetNormalizedTime();
	//Setter
	//アニメーションのブレンドを行うためのフラグ
	bool							IsBlending() { return mBlending; }


	void							Serialize(json& j) const override;
    void							Deserialize(const json& j) override;

    void							DrawCustomGUI(const std::vector<PropertyInfo>& properties) override;

    Component*						Clone(Entity* newOwner) const override;
};

