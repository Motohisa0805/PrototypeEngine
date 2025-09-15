#include "Stages00.h"

Stages00::Stages00()
	:ActorObject()
	, mDebugStage(nullptr)
	, mDebugStage2(nullptr)
	, mDebugStage3(nullptr)
	, mDebugStage4(nullptr)
	, mFloor00(nullptr)
	, mWall00(nullptr)
	, mWall01(nullptr)
	, mWall02(nullptr)
	, mTarget1(nullptr)
	, mTarget2(nullptr)
	, mTarget3(nullptr)
	, mSphere(nullptr)
	, mCube(nullptr)
	, mDice(nullptr)
	, mDamageTrap(nullptr)
	, mHealthObject(nullptr)
	, mCapsule(nullptr)
	, mPortal(nullptr)
	, mParticleTestActor(nullptr)
{
}

void Stages00::Initialize()
{
	Quaternion q;
	Vector3 pos;
	mDebugStage = new MeshActor();
	mDebugStage->Load("DebugStage.fbx");
	pos = Vector3(0.0f, -1.0f, 0.0f);
	mDebugStage->SetLocalPosition(pos);
	mDebugStage->SetActorTag(ActorTag::Ground);
	mDebugStage->AddBoxCollider();



	mFloor00 = new MeshActor();
	mFloor00->Load("Floor_00.fbx");
	pos = Vector3(-20.0f, -1.5f, 0.0f);
	mFloor00->SetLocalPosition(pos);
	mFloor00->SetActorTag(ActorTag::Ground);
	mFloor00->AddBoxCollider();
	mDebugStage2 = new MeshActor();
	mDebugStage2->Load("DebugStage.fbx");
	pos = Vector3(0.0f, 9.5f, 10.5f);
	mDebugStage2->SetLocalPosition(pos);
	q = Quaternion(Vector3::UnitX, -Math::PiOver2);
	mDebugStage2->SetLocalRotation(q);
	mDebugStage2->AddBoxCollider();

	mWall00 = new MeshActor();
	mWall00->Load("Wall_00.fbx");
	pos = Vector3(-20.0f, 4.5f, 11.0f);
	mWall00->SetLocalPosition(pos);
	mWall00->SetLocalRotation(Quaternion(Vector3::UnitY, -Math::PiOver2));
	mWall00->AddBoxCollider();

	mDebugStage3 = new MeshActor();
	mDebugStage3->Load("DebugStage.fbx");
	pos = Vector3(0.0f, 9.5f, -10.5f);
	mDebugStage3->SetLocalPosition(pos);
	q = Quaternion(Vector3::UnitX, Math::PiOver2);
	mDebugStage3->SetLocalRotation(q);
	mDebugStage3->AddBoxCollider();

	mWall01 = new MeshActor();
	mWall01->Load("Wall_00.fbx");
	pos = Vector3(-20.0f, 4.5f, -11.0f);
	mWall01->SetLocalPosition(pos);
	mWall01->SetLocalRotation(Quaternion(Vector3::UnitY, Math::PiOver2));
	mWall01->AddBoxCollider();

	mDebugStage4 = new MeshActor();
	mDebugStage4->Load("DebugStage.fbx");
	pos = Vector3(10.5f, 9.5f, 0.0f);
	mDebugStage4->SetLocalPosition(pos);
	q = Quaternion(Vector3::UnitX, -Math::PiOver2);
	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitY, -Math::PiOver2));
	mDebugStage4->SetLocalRotation(q);
	mDebugStage4->AddBoxCollider();

	mWall02 = new MeshActor();
	mWall02->Load("Wall_00.fbx");
	pos = Vector3(-30.0f, 4.5f, 0.0f);
	mWall02->SetLocalPosition(pos);
	mWall02->SetLocalRotation(Quaternion(Vector3::UnitY, Math::Pi));
	mWall02->AddBoxCollider();


	// 的オブジェクト生成
	mTarget1 = new TargetActor();
	mTarget1->SetLocalPosition(Vector3(-25.0f, 5.0f, 0.0f));

	mTarget2 = new TargetActor();
	mTarget2->SetLocalPosition(Vector3(-25.0f, 5.0f, 5.0f));

	mTarget3 = new TargetActor();
	mTarget3->SetLocalPosition(Vector3(-25.0f, 5.0f, -5.0f));

	mCube = new MeshActor();
	mCube->Load("Cube.fbx");
	mCube->SetLocalPosition(Vector3(0.0f, 0.0f, 6.0f));
	mCube->AddBoxCollider();

	mCapsule = new MeshActor();
	mCapsule->Load("Capsule.fbx");
	mCapsule->SetLocalPosition(Vector3(4.0f, 0.5f, 6.0f));
	mCapsule->AddCapsuleCollider();
	mCube->AddChildActor(mCapsule);


	mSphere = new MeshActor();
	mSphere->Load("Sphere.fbx");
	mSphere->SetLocalPosition(Vector3(-4.0f, 0.0f, 6.0f));
	mSphere->AddSphereCollider();
	mSphere->AddChildActor(mCapsule);
	mCube->AddChildActor(mSphere);

	mDice = new MeshActor();
	mDice->Load("TestSpiritBot.fbx");
	mDice->SetLocalPosition(Vector3(8.0f, 0.0f, 6.0f));
	mDice->AddBoxCollider();

	mDamageTrap = new MeshActor();
	mDamageTrap->Load("DamageTrap.fbx");
	mDamageTrap->SetLocalPosition(Vector3(-20.0f, 0.7f, -4.0f));
	mDamageTrap->SetActorTag(ActorTag::Enemy);
	mDamageTrap->AddSphereCollider();
	mDamageTrap->SetColliderMode(false);

	mHealthObject = new MeshActor();
	mHealthObject->Load("Health.fbx");
	mHealthObject->SetLocalScale(0.5f);
	mHealthObject->SetLocalPosition(Vector3(-20.0f, 1.0f, 4.0f));
	mHealthObject->SetActorTag(ActorTag::Recovery);
	mHealthObject->AddBoxCollider();
	mHealthObject->SetColliderMode(false);

	mPortal = new PortalActor();
	mPortal->SetLocalPosition(Vector3(0.0f, 1.0f, -6.0f));

	mParticleTestActor = new ParticleTestActor();
	mParticleTestActor->SetLocalPosition(Vector3(4.0f, 1.0f, -6.0f));
}

void Stages00::UpdateActor(float deltaTime)
{
	//簡易的なY軸回転処理
	float rotationAmountY = mHealthObject->GetRotationAmountY();
	rotationAmountY += 5 * Time::gDeltaTime;
	mHealthObject->SetRotationAmountY(rotationAmountY);
	Quaternion r = Quaternion(Vector3::UnitY, rotationAmountY);
	mHealthObject->SetLocalRotation(r);

	//簡易的なY軸回転処理
	rotationAmountY = mDamageTrap->GetRotationAmountY();
	rotationAmountY += 5 * Time::gDeltaTime;
	mDamageTrap->SetRotationAmountY(rotationAmountY);
	r = Quaternion(Vector3::UnitY, rotationAmountY);
	mDamageTrap->SetLocalRotation(r);
}
