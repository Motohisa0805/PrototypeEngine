#pragma once
#include "MeshActor.h"
#include "SphereCollider.h"
#include "BoxCollider.h"

//�e��Actor�N���X
class BulletActor : public ActorObject
{
private:
	//�e�̃��b�V��
	MeshActor*				mBullet;
	// �e�̗͂̕���
	Vector3					mPowerDirection;
	
	SphereCollider*			mSphereCollider;
	// �{�b�N�X�R���C�_�[
	BoxCollider*			mBoxCollider;
	// �e�̔j��܂ł̎���
	float					mDestroyCount;
	// �e�̗�
	float					mPower;
public:
							BulletActor(Vector3 powerDir,Vector3 position);
							~BulletActor();

	void					UpdateActor(float deltaTime) override;

	void					OnCollisionEnter(class ActorObject* target) override;
};

