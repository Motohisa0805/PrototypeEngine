#include "FPSActor.h"
#include "FPSCanvas.h"

FPSActor::FPSActor()
	:ActorObject()
{
	mName = "FPSActor";

	mActorTag = ActorTag::Player;

	mBasicInput = new BasicInputAction(this);

	mRigidbody = new Rigidbody(this);
	mRigidbody->SetBounciness(0.0f);

	mAudioComp = new AudioComponent(this);
	mLastFootstep = 0.0f;
	mFootstep = mAudioComp->LoadAudio("event:/Footstep");
	mFootstep.SetPaused(true);

	mFPSCamera = new FPSCamera(this);

	// ボックスの当たり判定の機能を追加
	mBoxCollider = new BoxCollider(this);
	OBB myOBB(mPosition, mRotation, Vector3(1.0f,1.5f,1.0f));
	mBoxCollider->SetObjectOBB(myOBB);
	mBoxCollider->SetStaticObject(false);

	mMaxHP = 100.0f;
	mHP = mMaxHP;
	//イベント追加
	mEvent.Add
	(
		[this]() 
		{
			mFootstep.SetPaused(false); 
			mFootstep.ResetStart(); 
		}
	);


	mGunActor = new GunActor();
	AddChildActor(mGunActor);
	mGunActor->SetLocalPosition(Vector3(0.5f,-0.25f,0.5f));
}

void FPSActor::FixedUpdateActor(float deltaTime){}


void FPSActor::UpdateActor(float deltaTime)
{
	ActorObject::UpdateActor(deltaTime);

	if (!mBasicInput->GetJumping()) 
	{
		// 足音のSEを再生する処理
		mLastFootstep -= deltaTime;
		if ((!Math::NearZero(mBasicInput->GetForwardSpeed()) ||
			!Math::NearZero(mBasicInput->GetStrafeSpeed())) &&
			mLastFootstep <= 0.0f)
		{
			mEvent.Invoke();
			mLastFootstep = 0.5f;
		}
	}

	mBasicInput->SetJumping(true);
	// Make a line segment
	const float cAimDist = 3.5f;
	Vector3 start = mPosition;
	//start.y += 0.5f;
	Vector3 dir = GetUp();
	dir *= -0.5f;
	LineSegment l(start, start + dir * cAimDist);
	// Segment cast
	PhysWorld::CollisionInfo info;
	ActorTag tag = ActorTag::Ground;
	if (mGame->GetPhysWorld()->RayCast(l, info,tag))
	{
		if (info.mActor->GetActorTag() == ActorTag::Ground)
		{
			mBasicInput->SetJumping(false);
		}
	}

	GunObjectUpdate(deltaTime);
}

void FPSActor::GunObjectUpdate(float deltaTime)
{
	Quaternion q = Quaternion(mGunActor->GetRight(), mFPSCamera->GetPitch());
	mGunActor->SetLocalRotation(q);
}

void FPSActor::ActorInput(const struct InputState& keys)
{
	mBasicInput->MoveInputUpdate(keys);
	mFPSCamera->CameraInputUpdate();
}

void FPSActor::SetFootstepSurface(float value)
{
	//ここで一時停止します。
	// FMODでパラメーターを設定した方法を変更すると、足音が再生されるため
	mFootstep.SetPaused(true);
	mFootstep.SetParameter("Surface", value);
}

void FPSActor::SetVisible(bool visible)
{
	mMeshComp->SetVisible(visible);
}
void FPSActor::OnCollisionEnter(ActorObject* target)
{
	/*
	if (target->GetActorTag() == ActorTag::Enemy)
	{
		mHP -= 10.0f;
		if (mHP < 0)
		{
			mHP = 0;
		}
		mGame->GetHUD()->GetHelthBar()->SetFillAmount(mHP / mMaxHP);
	}
	else if (target->GetActorTag() == ActorTag::Recovery)
	{
		mHP += 10.0f;
		if (mHP > mMaxHP)
		{
			mHP = mMaxHP;
		}
		mGame->GetHUD()->GetHelthBar()->SetFillAmount(mHP / mMaxHP);
	}
	*/
}

void FPSActor::OnCollisionExit(ActorObject* target)
{

}
