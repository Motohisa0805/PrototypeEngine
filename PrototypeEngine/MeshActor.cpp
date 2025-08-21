#include "MeshActor.h"

MeshActor::MeshActor()
	: ActorObject()
	, mMeshRenderer(nullptr)
	, mBoxCollider(nullptr)
	, mSphereCollider(nullptr)
	, mCapsuleCollider(nullptr)
{
}

MeshActor::~MeshActor()
{
}

void MeshActor::Load(string filePath)
{
	mMeshRenderer = new MeshRenderer(this);
	vector<class Mesh*> mesh = EngineWindow::GetRenderer()->GetMeshs(filePath);
	mMeshRenderer->SetMeshs(mesh);
}

void MeshActor::SetColliderMode(bool active)
{
	if (mBoxCollider)
	{
		mBoxCollider->SetCollider(active);
	}

	if (mSphereCollider)
	{
		mSphereCollider->SetCollider(active);
	}

	if (mCapsuleCollider)
	{
		mCapsuleCollider->SetCollider(active);
	}
}

void MeshActor::SetStaticMode(bool active)
{
	if (mBoxCollider)
	{
		mBoxCollider->SetStaticObject(active);
	}

	if (mSphereCollider)
	{
		mSphereCollider->SetStaticObject(active);
	}

	if (mCapsuleCollider)
	{
		mCapsuleCollider->SetStaticObject(active);
	}
}

void MeshActor::AddBoxCollider()
{
	BoxCollider* box = new BoxCollider(this);
	box->SetObjectBox(mMeshRenderer->GetMeshs()[0]->GetBoxs()[0]);
	box->SetObjectOBB(mMeshRenderer->GetMeshs()[0]->GetOBBBoxs()[0]);
	mBoxCollider = box;
}

void MeshActor::AddSphereCollider()
{
	SphereCollider* sphereCollider = new SphereCollider(this);
	Vector3 max = mMeshRenderer->GetMeshs()[0]->GetBoxs()[0].mMax;
	Vector3 min = mMeshRenderer->GetMeshs()[0]->GetBoxs()[0].mMin;
	Vector3 extent = (max - min) * 0.5f;
	extent.Length();
	Sphere sphere(mPosition, extent.Length());
	sphereCollider->SetObjectSphere(sphere);

	mSphereCollider = sphereCollider;
}

void MeshActor::AddCapsuleCollider()
{
	OBB obb = mMeshRenderer->GetMeshs()[0]->GetOBBBoxs()[0];
	// 中心と回転
	Vector3 center = obb.mCenter;
	Quaternion rot = obb.mRotation;

	// ローカル軸方向
	Vector3 up = Vector3::Transform(Vector3::UnitY, rot); // ローカルY軸を世界空間に

	// 半径 = X/Zの小さい方
	float radius = std::min(obb.mExtents.x, obb.mExtents.z);

	// 線分の長さ = Y軸方向の長さから両端の半球を除いた部分
	float lineHalfLength = obb.mExtents.y - radius;

	// 始点・終点を中心から上下に延ばす
	Vector3 start = center - up * lineHalfLength;
	Vector3 end = center + up * lineHalfLength;

	Capsule capsule(start, end,radius);

	CapsuleCollider* capsuleCollider = new CapsuleCollider(this);
	capsuleCollider->SetObjectCapsule(capsule);
	mCapsuleCollider = capsuleCollider;
}

const AABB MeshActor::GetBoxAABB()
{
	AABB aabb(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f));
	if (mMeshRenderer)
	{
		aabb = mMeshRenderer->GetMeshs()[0]->GetBoxs()[0];
	}
	return aabb;
}

const OBB MeshActor::GetBoxOBB()
{
	OBB obb(mPosition,mRotation,mScale);
	if (mMeshRenderer)
	{
		obb = mMeshRenderer->GetMeshs()[0]->GetOBBBoxs()[0];
	}
	return obb;
}
