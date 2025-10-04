#include "BallActor.h"
#include "GameWinMain.h"
#include "BaseScene.h"
#include "Renderer.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "BallMove.h"
#include "AudioComponent.h"

BallActor::BallActor()
	:ActorObject()
	, mLifeSpan(2.0f)
{
	//SetScale(10.0f);
	MeshRenderer* mc = new MeshRenderer(this);
	Mesh* mesh = GetGame()->GetWinMain()->GetRenderer()->GetMesh("Sphere.gpmesh");
	mc->SetMesh(mesh);
	mMyMove = new BallMove(this);
	mMyMove->SetForwardSpeed(1500.0f);
	mAudioComp = new AudioComponent(this);
}

void BallActor::UpdateActor(float deltaTime)
{
	ActorObject::UpdateActor(deltaTime);

	mLifeSpan -= deltaTime;
	if (mLifeSpan < 0.0f)
	{
		SetState(EDead);
	}
}

void BallActor::SetPlayer(ActorObject* player)
{
	mMyMove->SetPlayer(player);
}

void BallActor::HitTarget()
{
	mAudioComp->LoadAudio("event:/Ding");
}