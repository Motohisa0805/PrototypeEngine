#pragma once
#include "Component.h"
#include "Shader.h"
#include "Actor.h"
#include "Texture.h"
#include "Random.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//パーティクルの構造体
struct ParticleStruct
{
	Vector3 mPosition;
	Vector3 mVelocity;
	float	mMaxLifetime;
	float	mLifetime;
	float	mSize;
	bool	mActive;
	Vector4 mColor;
};
// パーティクルシステムクラス
class ParticleSystem : public Component
{
protected:
	// パーティクルのインスタンス
	vector<ParticleStruct*> mParticle;
	// パーティクルのテクスチャ
	class Texture*			mParticleTexture;
	//
	bool					mIsAlphaFade;
	// パーティクルのループフラグ
	bool					mIsLoop; 
	// パーティクルの可視性フラグ
	bool					mVisible;
	// パーティクルの破棄フラグ
	bool					mIsDestroyed;
	// 最大パーティクル数
	int						mMaxParticleCount; 
	// パーティクルのライフタイム
	float					mParticleLifeTime;
	// パーティクルの全体ライフタイム
	float					mParticleAllLifeTime;
	// パーティクルの発射速度
	Vector3					mParticleEmitSpeed;
	// パーティクルのサイズ
	// パーティクルの最大サイズ
	float					mParticleMaxSize;
	// パーティクルの最小サイズ
	float					mParticleMinSize;
	// パーティクルのデフォルト色
	Vector4					mDefaultColor; // デフォルトは白色
	// パーティクルの発射間隔
	float					mEmitInterval = 0.02f; // 50個/秒
	// パーティクルの発射タイマー
	float					mEmitTimer = 0.0f;
public:
	ParticleSystem(class ActorObject* owner);
	~ParticleSystem();
	// パーティクルシステムの更新
	void						Update(float deltaTime)override;
	// パーティクルの描画
	virtual void				Draw(class Shader* shader);
	// テクスチャの読み込み
	void						LoadTexture(string name);
	//速度を与える
	Vector3						AddVelocity();
	// パーティクルの発射
	void						Emit();
	// パーティクルを取得
	vector<ParticleStruct*>		GetParticle() const { return mParticle; }
	//パーティクルの可視性の取得
	bool						IsVisible() const { return mVisible; }
	// 可視性の設定
	void						SetVisible(bool visible) { mVisible = visible; }
	// アルファフェードの取得
	bool 						IsAlphaFade() const { return mIsAlphaFade; }
	// アルファフェードの設定
	void 						SetAlphaFade(bool alphaFade) { mIsAlphaFade = alphaFade; }
	// ループフラグの取得
	bool 						IsLoop() const { return mIsLoop; }
	// ループフラグの設定
	void						SetLoop(bool loop) { mIsLoop = loop; }
	// 破棄フラグの取得
	bool 						IsDestroyed() const { return mIsDestroyed; }
	// 破棄フラグの設定
	void						SetDestroyed(bool destroyed) { mIsDestroyed = destroyed; }
	// 最大パーティクル数の取得
	void						SetParticleSpeed(Vector3 speed);
	// パーティクルの速度設定
	void						SetColor(Vector4 color);
	// パーティクルの色設定
	void						SetParticleLifeTime(float lifeTime) { mParticleLifeTime = lifeTime; }
	
	float						GetParticleMaxSize() const { return mParticleMaxSize; }

	float						GetParticleMinSize() const { return mParticleMinSize; }

	void						SetParticleSize(float min,float max) 
	{
		mParticleMinSize = min;
		mParticleMaxSize = max; 
	}

	ParticleStruct*				GetInactiveParticleOrCreateNew();

	void SetEmitInterval(float interval);
};

