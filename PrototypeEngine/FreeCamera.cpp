#include "FreeCamera.h"

FreeCamera::FreeCamera(ActorObject* owner)
	:BaseCamera(owner)
	, mUp(Vector3::UnitY)
{
	mName = "FreeCamera";
}

void FreeCamera::Initiailze()
{

}

void FreeCamera::Update(float deltaTime)
{
	BaseCamera::Update(deltaTime);

}

void FreeCamera::SetCameraPosition(const Vector3& eye, const Vector3& target)
{
	Matrix4 view = Matrix4::CreateLookAt(eye, target, mUp);
	SetViewMatrix(view);
}
