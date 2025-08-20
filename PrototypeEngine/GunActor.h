#pragma once
#include "Actor.h"
#include "MeshActor.h"
#include "BulletActor.h"
#include "SoundEventClip.h"
#include "AudioSystem.h"
#include "AudioComponent.h"

class GunActor : public ActorObject
{
private:
	//�e�̃��b�V��
	MeshActor*				mGunMesh;
	//�e��SE���Đ����邽�߂̃R���|�[�l���g
	AudioComponent*			mAudioComp;

public:
							GunActor();

	void					FixedUpdateActor(float deltaTime)override;

	void					UpdateActor(float deltaTime) override;

	void					ActorInput(const struct InputState& keys) override;

	void					Fire();
};

