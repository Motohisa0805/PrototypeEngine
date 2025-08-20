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
	//銃のメッシュ
	MeshActor*				mGunMesh;
	//銃のSEを再生するためのコンポーネント
	AudioComponent*			mAudioComp;

public:
							GunActor();

	void					FixedUpdateActor(float deltaTime)override;

	void					UpdateActor(float deltaTime) override;

	void					ActorInput(const struct InputState& keys) override;

	void					Fire();
};

