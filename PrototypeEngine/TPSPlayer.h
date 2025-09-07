#pragma once
#include "SkeletonMeshActor.h"
#include "BoxCollider.h"
#include "FollowCamera.h"
#include "FollowObjectMovement.h"

//TPS�v���C���[�̗p�̃N���X
//��Ƀ��f���ǂݍ��݁A�A�j���[�V�����ǂݍ��݁A���͈ړ����Ǘ�����N���X
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
	SkeletonMeshActor*				mSkeletonMeshActor;

	FollowCamera*					mFollowCamera;

	FollowObjectMovement*			mMovement;

	BoxCollider*					mBoxCollider;

public:
									TPSPlayer();

	void							UpdateActor(float deltaTime)override;

	void							ActorInput(const struct InputState& keys) override;
};

