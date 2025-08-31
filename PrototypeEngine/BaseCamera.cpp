#include "BaseCamera.h"
#include "Actor.h"
#include "Renderer.h"
#include "GameWinMain.h"
#include "BaseScene.h"
#include "AudioSystem.h"

BaseCamera::BaseCamera(ActorObject* owner, int updateOrder)
	:Component(owner, updateOrder)
	, mCameraForward(Vector3::Zero)
	, mCameraRight(Vector3::Zero)
	, mCameraUp(Vector3::Zero)
	, mCameraYawRot()
	, mIsMain(false)
	, mView()
{
	mGame->AddCamera(this);
}

BaseCamera::~BaseCamera()
{
	mGame->RemoveCamera(this);
}

void BaseCamera::SetViewMatrix(const Matrix4& view)
{
	mView = view;
}