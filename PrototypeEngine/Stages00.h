#pragma once
#include "MeshActor.h"
#include "TargetActor.h"
#include "PortalActor.h"
#include "ParticleTestActor.h"

class Stages00 : public ActorObject
{
private:
	MeshActor* mDebugStage;

	MeshActor* mDebugStage2;

	MeshActor* mDebugStage3;

	MeshActor* mDebugStage4;

	MeshActor* mFloor00;

	MeshActor* mWall00;

	MeshActor* mWall01;
	
	MeshActor* mWall02;

	TargetActor* mTarget1;
	TargetActor* mTarget2;
	TargetActor* mTarget3;

	MeshActor* mSphere;

	MeshActor* mCube;

	MeshActor* mDice;

	MeshActor* mDamageTrap;

	MeshActor* mHealthObject;

	MeshActor* mCapsule;

	PortalActor* mPortal;

	ParticleTestActor* mParticleTestActor;
public:
							Stages00();

	void					Initialize();

	void					UpdateActor(float deltaTime) override;
};

