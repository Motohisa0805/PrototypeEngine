#pragma once
#include "Actor.h"
#include "ParticleSystem.h"

class ParticleTestActor : public ActorObject
{
private:
	ParticleSystem* mParticleSystem;
public:
	ParticleTestActor();
	~ParticleTestActor();
};

