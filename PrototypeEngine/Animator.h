#pragma once
#include "MatrixPalette.h"
#include "Animation.h"
#include "Skeleton.h"
#include "AnimatorControllerParameters.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//複数個あるアニメーションを管理するクラス
//Unityの「Animator」を意識して設計
//アニメーションの再生、ブレンドなどを行う
class Animator : public Component
{
private:
	//現在再生中のアニメーション名
    string									mCurrentStateName;

	//アニメーションデータを管理する「AnimatiorController」
    filesystem::path						mControllerFilePath;
    AnimatorControllerParameters			mControllerData;
    std::unordered_map<string, Animation*>  mStateAnimations;
	//保存データ
	//スケルトン(アバター)
    filesystem::path				mSkeletonFilePath;
	SkeletonData*					mSkeleton;
	//非保存データ
	vector<ActorObject*>			mBones;
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

	//メモリ管理・互換性のため残す
	//************************************************
    vector<AnimInfo>				mAnimationInfo;
	//アニメーションを配列で持ってる変数
	vector<Animation*>				mAnimations;
	//*************************************************
    void								CheckStateTransitions();
public:
    Animator(Entity* owner);
	~Animator();
    bool								LoadController(const string& filePath);
	//アニメーション読み込み処理
	bool								Load(const string& fileName,bool animLoop = 0,bool rootMotion = 0);
	// アニメーションの更新処理
	void								Update(float deltaTime)override;
	// アニメーションを再生します。
	// アニメーションの長さを返します。
	float								PlayAnimation(Animation* anim);
	// ブレンドアニメーションを再生します。
	float								PlayBlendAnimation(Animation* anim);
	//アニメーションの配列のGetter
	vector<Animation*>					GetAnimations() { return mAnimations; }
    void								AddAnimation(Animation* anim);
	//スケルトンのGetter
	SkeletonData*						GetSkeleton() { return mSkeleton; }

	filesystem::path					GetControllerFilePath() { return mControllerFilePath; }

	AnimatorControllerParameters&		GetControllerData() { return mControllerData; }
	const AnimatorControllerParameters&	GetControllerData() const { return mControllerData; }

	//スケルトンのSetter
	void								ReloadBones(ActorObject* rootbone);
    void								LoadSkeletonData(const string& fileName,ActorObject* rootBone);
	//現在再生中のアニメーションのGetter
	Animation*							GetAnimation() { return mAnimation; }
	//ブレンドアニメーションのGetter
	Animation*							GetBlendAnimation() { return mBlendAnimation; }
	//アニメーションの倍率
	float								GetAnimPlayRate() { return mAnimPlayRate; }
	//アニメーションの倍率をセットする関数
	void								SetAnimPlayRate(float playRate) { mAnimPlayRate = playRate; }
	//現在再生中のアニメーションの時間
	float								GetAnimTime() { return mAnimTime; }
	//ブレンドアニメーションの時間
	float								GetBlendAnimTime() { return mBlendAnimTime; }
	// 現在のブレンド経過時間
	float								GetBlendElapsed() { return mBlendElapsed; }
	//ブレンドアニメーションの時間をセットする関数
	void								SetBlendElapsed(float elapsed) { mBlendElapsed = elapsed; }
	//現在のアニメーションの時間を正規化した値を出力する関数
	float								GetNormalizedTime();
	//Setter
	//アニメーションのブレンドを行うためのフラグ
	bool								IsBlending() { return mBlending; }


	void								Serialize(json& j) const override;
    void								Deserialize(const json& j) override;
    void								DeserializeAfterParentChildBuild() override;

    void								DrawCustomGUI(const std::vector<PropertyInfo>& properties) override;

    Component*							Clone(Entity* newOwner) const override;
};

