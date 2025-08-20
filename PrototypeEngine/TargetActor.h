#pragma once
#include "MeshActor.h"
#include "BoxCollider.h"
#include "BulletActor.h"
#include "SoundEventClip.h"
#include "AudioSystem.h"
#include "AudioComponent.h"
#include "TargetComponent.h"

class TargetActor : public ActorObject
{
private:
	MeshActor*		mMesh;

	BoxCollider*	mBoxCollider;

	float			mHitCoolDownCount;

	float			mRotateCount;

	AudioComponent* mAudioComp;
public:
	TargetActor();

	void RotateTarget();

	void UpdateActor(float deltaTime)override;

	void OnCollisionEnter(class ActorObject* target) override;
};

