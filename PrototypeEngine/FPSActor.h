#pragma once
#include "Actor.h"
#include "SoundEventClip.h"
#include "BasicInputAction.h"
#include "AudioSystem.h"
#include "AudioComponent.h"
#include "BoxCollider.h"
#include "FPSCamera.h"
#include "GunActor.h"

//一人称視点のプレイヤー操作クラス
//現在ゲーム内の移動に使用しています。
class FPSActor : public ActorObject
{
private:
	//入力クラス
	BasicInputAction*		mBasicInput;
	//プレイヤー用のサウンド
	AudioComponent*			mAudioComp;
	//Mesh
	MeshRenderer*			mMeshComp;
	//一人称カメラ
	FPSCamera*				mFPSCamera;
	//コライダー
	BoxCollider*			mBoxCollider;
	//足音SE
	SoundEventClip			mFootstep;
	//足音の間隔
	float					mLastFootstep;
	//足音のイベントを呼び出すコールバック変数
	Event<void()>			mEvent;
	//簡易的な最大HPの変数
	float					mMaxHP;
	//簡易的なHPの変数
	float					mHP;

	GunActor*				mGunActor;
public:
							FPSActor();

	void					FixedUpdateActor(float deltaTime)override;

	void					UpdateActor(float deltaTime) override;

	void					GunObjectUpdate(float deltaTime);
	
	void					ActorInput(const struct InputState& keys) override;

	void					SetFootstepSurface(float value);

	void					SetVisible(bool visible);

	void					OnCollisionEnter(ActorObject* target) override;

	void					OnCollisionExit(ActorObject* target) override;
};