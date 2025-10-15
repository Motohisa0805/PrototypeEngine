#include "FreeCamera.h"

FreeCamera::FreeCamera(ActorObject* owner)
	:BaseCamera(owner)
	, mUp(Vector3::UnitY)
{
	mName = "FreeCamera";
}

void FreeCamera::Initiailze()
{
	MoveCameraView();
}

void FreeCamera::Update(float deltaTime)
{
	BaseCamera::Update(deltaTime);
	MoveCameraView();
}

void FreeCamera::SetCameraPosition(const Vector3& eye, const Vector3& target)
{
	Matrix4 view = Matrix4::CreateLookAt(eye, target, mUp);
	SetViewMatrix(view);
}

void FreeCamera::DrawGUI()
{
	MoveCameraView();

}

void FreeCamera::MoveCameraView()
{
	// ���L�҂��s�b�`�N�H�[�^�j�I���őO���ɉ�]������B
	Vector3 viewForward = Vector3::Transform(mOwner->GetForward(), mOwner->GetLocalRotation());

	Vector3 cameraPos = mOwner->GetLocalPosition();
	Vector3 target = cameraPos + viewForward * 100.0f;
	// �s�b�`�N�H�[�^�j�I������]�B
	Vector3 up = Vector3::Transform(Vector3::UnitY, mOwner->GetLocalRotation());

	// �}�g���b�N�X���쐬���A�r���[�Ƃ��Đݒ肵�܂��B
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	WindowRenderProperty::SetViewEye(cameraPos);
	WindowRenderProperty::SetViewTarget(target);
	WindowRenderProperty::SetViewUp(up);
	SetViewMatrix(view);
}
