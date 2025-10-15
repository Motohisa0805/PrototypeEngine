#include "TargetActor.h"

TargetActor::TargetActor()
	:ActorObject()
	, mMesh(nullptr)
	, mHitCoolDownCount(0)
	, mRotateCount(2.0f)
{
	mName = "TargetActor";

	mActorTag = ActorTag::Enemy;

	mMesh = new MeshActor();
	mMesh->Load("Target.fbx");
	AddChildActor(mMesh);
	
	BoxCollider* box = new BoxCollider(this);
	box->SetObjectBox(mMesh->GetBoxAABB());
	box->SetObjectOBB(mMesh->GetBoxOBB());
	mBoxCollider = box;

	mAudioComp = new AudioComponent(this);

	new TargetComponent(this);
}

void TargetActor::RotateTarget()
{
	/*
	float y = GetRotationAmountY();
	y += 0.1f;
	Quaternion q = Quaternion(Vector3::UnitY, y);
	SetLocalRotation(q);
	SetRotationAmountY(y);
	*/
}

void TargetActor::UpdateActor(float deltaTime)
{
	if (mHitCoolDownCount > 0)
	{
		RotateTarget();

		mHitCoolDownCount -= deltaTime;
	}
	else
	{
		LookAt(mGame->GetPlayer()->GetPosition());
	}
}

void TargetActor::OnCollisionEnter(ActorObject* target)
{
	BulletActor* bullet = (BulletActor*)target;
	if (bullet == nullptr) { return; }
	if (mHitCoolDownCount > 0) { return; }
	mHitCoolDownCount = mRotateCount;
	mAudioComp->LoadAudio("event:/HitTarget");
}
