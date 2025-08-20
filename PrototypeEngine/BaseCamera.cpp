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
{
	mGame->SetMainCamera(this);
}

void BaseCamera::SetViewMatrix(const Matrix4& view)
{
	// ビュー行列をレンダラーとオーディオシステムに渡す
	BaseScene* game = mOwner->GetGame();
	GameWinMain::GetRenderer()->SetViewMatrix(view);
	game->GetAudioSystem()->SetListener(view);
}