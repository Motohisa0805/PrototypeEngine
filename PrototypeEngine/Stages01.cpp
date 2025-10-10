#include "Stages01.h"

Stages01::Stages01()
	: ActorObject()
{
	mName = "Stages01";
}

void Stages01::Initialize()
{
	Quaternion q;
	mTestCharacter = new SkeletonMeshActor();
	mTestCharacter->Load("goblin_d_shareyko.fbx");
	mTestCharacter->SetLocalPosition(Vector3(-2.0f, -0.5f, 4.0f));

	mTestCharacter->SetAnimatorName("TestCharacter");
	mTestCharacter->GetAnimator()->Load("Idle.fbx", true);
	mTestCharacter->GetAnimator()->Load("Running.fbx", true);
	mTestCharacter->GetAnimator()->Load("Jumping.fbx");
	mTestCharacter->GetAnimator()->Load("Capoeira.fbx", true);
	mTestCharacter->GetAnimator()->Load("T-Pose.fbx", true);
	mTestCharacter->GetAnimator()->PlayAnimation(mTestCharacter->GetAnimator()->GetAnimations()[4]);

	mSmallCharacter = new SkeletonMeshActor();
	mSmallCharacter->Load("Paladin J Nordstrom.fbx");
	mSmallCharacter->SetLocalPosition(Vector3(-4.0f, -0.5f, 4.0f));

	mSmallCharacter->SetAnimatorName("SmallCharacter");
	mSmallCharacter->GetAnimator()->Load("Idle.fbx", true);
	mSmallCharacter->GetAnimator()->Load("Running.fbx", true);
	mSmallCharacter->GetAnimator()->Load("Jumping.fbx");
	mSmallCharacter->GetAnimator()->Load("Capoeira.fbx", true);
	mSmallCharacter->GetAnimator()->Load("T-Pose.fbx", true);
	mSmallCharacter->GetAnimator()->PlayAnimation(mSmallCharacter->GetAnimator()->GetAnimations()[4]);

	mFloor00 = new MeshActor();
	mFloor00->Load("Floor_00.fbx");
	Vector3 pos = Vector3(0.0f, -1.5f, 0.0f);
	mFloor00->SetLocalPosition(pos);
	mFloor00->SetActorTag(ActorTag::Ground);
	mFloor00->AddBoxCollider();

	mWall00 = new MeshActor();
	mWall00->Load("Wall_00.fbx");
	pos = Vector3(0.0f, 4.5f, 11.0f);
	mWall00->SetLocalPosition(pos);
	mWall00->SetLocalRotation(Quaternion(Vector3::UnitY, -Math::PiOver2));
	mWall00->AddBoxCollider();

	mWall01 = new MeshActor();
	mWall01->Load("Wall_00.fbx");
	pos = Vector3(0.0f, 4.5f, -11.0f);
	mWall01->SetLocalPosition(pos);
	mWall01->SetLocalRotation(Quaternion(Vector3::UnitY, Math::PiOver2));
	mWall01->AddBoxCollider();

	mWall02 = new MeshActor();
	mWall02->Load("Wall_00.fbx");
	pos = Vector3(-10.0f, 4.5f, 0.0f);
	mWall02->SetLocalPosition(pos);
	mWall02->SetLocalRotation(Quaternion(Vector3::UnitY, Math::Pi));
	mWall02->AddBoxCollider();

	mWall03 = new MeshActor();
	mWall03->Load("Wall_00.fbx");
	pos = Vector3(10.0f, 4.5f, 0.0f);
	mWall03->SetLocalPosition(pos);
	mWall03->SetLocalRotation(Quaternion(Vector3::UnitY, Math::Pi));
	mWall03->AddBoxCollider();

	mCubeActor00 = new CubeActor();
	mCubeActor00->SetLocalPosition(Vector3(5.0f, 0.0f, 5.0f));
	mCubeActor00->SetLocalScale(Vector3(2.0f, 1.0f, 2.0f));
	mCubeActor00->SetActorTag(ActorTag::Ground);

	mCubeActor01 = new CubeActor();
	mCubeActor01->SetLocalPosition(Vector3(5.0f, 0.0f, 0.0f));
	mCubeActor01->SetLocalScale(Vector3(3.0f, 2.0f, 3.0f));
	mCubeActor01->SetActorTag(ActorTag::Ground);

	mCubeActor02 = new CubeActor();
	mCubeActor02->SetLocalPosition(Vector3(5.0f, 0.5f, -5.0f));
	mCubeActor02->SetLocalScale(Vector3(3.0f, 3.0f, 3.0f));
	mCubeActor02->SetActorTag(ActorTag::Ground);

	mPortal = new PortalActor();
	mPortal->SetLocalPosition(Vector3(0.0f, 1.0f, -6.0f));
}

void Stages01::UpdateActor(float deltaTime)
{
}

void Stages01::ActorInput(const InputState& state)
{
	//テスト用：アニメーション切り替え
	if (state.Keyboard.GetKeyDown(KEY_1))
	{
		mTestCharacter->GetAnimator()->PlayBlendAnimation(mTestCharacter->GetAnimator()->GetAnimations()[4]);
		mSmallCharacter->GetAnimator()->PlayBlendAnimation(mSmallCharacter->GetAnimator()->GetAnimations()[4]);
	}
	else if (state.Keyboard.GetKeyDown(KEY_2))
	{
		mTestCharacter->GetAnimator()->PlayBlendAnimation(mTestCharacter->GetAnimator()->GetAnimations()[0]);
		mSmallCharacter->GetAnimator()->PlayBlendAnimation(mSmallCharacter->GetAnimator()->GetAnimations()[0]);
	}
	else if (state.Keyboard.GetKeyDown(KEY_3))
	{
		mTestCharacter->GetAnimator()->PlayBlendAnimation(mTestCharacter->GetAnimator()->GetAnimations()[1]);
		mSmallCharacter->GetAnimator()->PlayBlendAnimation(mSmallCharacter->GetAnimator()->GetAnimations()[1]);
	}
	else if (state.Keyboard.GetKeyDown(KEY_4))
	{
		mTestCharacter->GetAnimator()->PlayBlendAnimation(mTestCharacter->GetAnimator()->GetAnimations()[2]);
		mSmallCharacter->GetAnimator()->PlayBlendAnimation(mSmallCharacter->GetAnimator()->GetAnimations()[2]);
	}
	else if (state.Keyboard.GetKeyDown(KEY_5))
	{
		mTestCharacter->GetAnimator()->PlayBlendAnimation(mTestCharacter->GetAnimator()->GetAnimations()[3]);
		mSmallCharacter->GetAnimator()->PlayBlendAnimation(mSmallCharacter->GetAnimator()->GetAnimations()[3]);
	}
}
