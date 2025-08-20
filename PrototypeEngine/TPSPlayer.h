#pragma once
#include "SkeletonMeshActor.h"
#include "BoxCollider.h"
#include "FollowCamera.h"
#include "FollowObjectMovement.h"

//TPSプレイヤーの用のクラス
//主にモデル読み込み、アニメーション読み込み、入力移動を管理するクラス
class TPSPlayer : public ActorObject
{
public:
	enum State
	{
		Idle,
		Run,
		Jump,
		RunJump,
		Capoeira,
		TPose
	};
private:
	class SkeletonMeshActor*		mSkeletonMeshActor;

	class FollowCamera*				mFollowCamera;

	class FollowObjectMovement*		mMovement;

	class BoxCollider*				mBoxCollider;

public:
									TPSPlayer();

	void							UpdateActor(float deltaTime)override;

	void							ActorInput(const struct InputState& keys) override;
};

