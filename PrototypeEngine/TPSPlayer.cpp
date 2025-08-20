#include "TPSPlayer.h"

TPSPlayer::TPSPlayer()
	:ActorObject()
{
	mActorTag = ActorTag::Player;

	mSkeletonMeshActor = new SkeletonMeshActor();
	//mSkeletonMeshActor->Load("Y Bot.fbx");
	mSkeletonMeshActor->Load("Y Bot.fbx");
	//アニメーションの読み込み
	mSkeletonMeshActor->SetAnimatorName("TPSPlayer");
	mSkeletonMeshActor->GetAnimator()->Load("Sword And Shield Idle.fbx", true, true);
	mSkeletonMeshActor->GetAnimator()->Load("Running.fbx", true, true);
	mSkeletonMeshActor->GetAnimator()->Load("Jumping.fbx", false, false);
	mSkeletonMeshActor->GetAnimator()->Load("Jump.fbx", false, false);
	mSkeletonMeshActor->GetAnimator()->Load("Capoeira.fbx", true);
	mSkeletonMeshActor->GetAnimator()->Load("T-Pose.fbx", true);

	mSkeletonMeshActor->GetAnimator()->PlayAnimation(mSkeletonMeshActor->GetAnimator()->GetAnimations()[State::TPose]);
	
	//スケルトンメッシュを子オブジェクトに設定
	AddChildActor(mSkeletonMeshActor);
	mSkeletonMeshActor->SetLocalPosition(Vector3::Zero);
	mSkeletonMeshActor->SetLocalRotation(Quaternion());


	mFollowCamera = new FollowCamera(this);

	mMovement = new FollowObjectMovement(this);

	mRigidbody = new Rigidbody(this);

	// ボックスの当たり判定の機能を追加
	mBoxCollider = new BoxCollider(this);
	OBB obb(mPosition, mRotation, Vector3(0.5f, 0.5f, 0.5f));
	obb.mOffset = Vector3(0.0f, 0.6f, 0.0f);
	mBoxCollider->SetObjectOBB(obb);
	mBoxCollider->SetStaticObject(false);

	mPosition = Vector3::Zero;
}

void TPSPlayer::UpdateActor(float deltaTime)
{
	if (!mMovement->IsJumping())
	{
		if (mMovement->GetInputDirection().Length() > 0)
		{
			mSkeletonMeshActor->GetAnimator()->PlayBlendAnimation(mSkeletonMeshActor->GetAnimator()->GetAnimations()[State::Run]);
		}
		else if (mMovement->GetInputDirection().Length() <= 0)
		{
			mSkeletonMeshActor->GetAnimator()->PlayBlendAnimation(mSkeletonMeshActor->GetAnimator()->GetAnimations()[State::Idle]);
		}
	}

	mMovement->SetJumping(true);
	// Make a line segment
	const float cAimDist = 2.5f;
	Vector3 start = mPosition;
	start.y += 0.5f;
	Vector3 dir = GetUp();
	dir *= -0.2f;
	LineSegment l(start, start + dir * cAimDist);
	// Segment cast
	PhysWorld::CollisionInfo info;
	ActorTag tag = ActorTag::Ground;
	if (mGame->GetPhysWorld()->RayCast(l, info, tag))
	{
		if (info.mActor->GetActorTag() == ActorTag::Ground)
		{
			mMovement->SetJumping(false);
		}
	}
}

void TPSPlayer::ActorInput(const InputState& keys)
{
	if (keys.Keyboard.GetKeyDown(SDL_SCANCODE_SPACE) && !mMovement->IsJumping())
	{
		mSkeletonMeshActor->GetAnimator()->PlayBlendAnimation(mSkeletonMeshActor->GetAnimator()->GetAnimations()[State::RunJump]);
	}
	mMovement->MoveInputUpdate(keys);
}
