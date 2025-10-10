#include "ParticleTestActor.h"

ParticleTestActor::ParticleTestActor()
	:ActorObject()
{
	mName = "ParticleTestActor";

	mParticleSystem = new ParticleSystem(this);
	mParticleSystem->LoadTexture("Particle.png");
	mParticleSystem->SetLoop(true);
	mParticleSystem->SetVisible(true);
	mParticleSystem->SetAlphaFade(true);
	mParticleSystem->SetParticleLifeTime(0.5f);
	mParticleSystem->SetParticleSize(0.1f,1.0f);
}

ParticleTestActor::~ParticleTestActor()
{
}
