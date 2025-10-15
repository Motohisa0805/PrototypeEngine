#include "SceneEditorCamera.h"
#include "SceneViewPanel.h"

SceneEditorCamera::SceneEditorCamera()
	: mIsViewDirty(false)
	, mYawSpeed(0.0f)
	, mPitchSpeed(0.0f)
	, mMaxPitch(Math::Pi / 3.0f)
	, mPitch(0.0f)
	, mMouseSensitivityX(500.0f)
	, mMouseSensitivityY(500.0f)
	, mForwardSpeed(0.0f)
	, mStrafeSpeed(0.0f)
	, mMaxSpeed(16.0f)
	, mMinSpeed(2.0f)
	, mSpeed(8.0f)
{
}

void SceneEditorCamera::Update()
{
	if (!mIsViewDirty) { return; }
	// SceneView�p�l���Ƀ}�E�X������Ă��Ȃ��ꍇ�A�������Ȃ�
	if (!GUIWinMain::GetSceneViewPanel()->IsMouseHovered()) { return; }
	//���_��]
	if (!Math::NearZero(mYawSpeed))
	{
		Quaternion rot = mLocalRotation;
		float angular = mYawSpeed * Time::gUnscaledDeltaTime;
		// Create quaternion for incremental rotation
		// (Rotate about up axis)
		Quaternion inc(Vector3::UnitY, angular);
		// Concatenate old and new quaternion
		rot = Quaternion::Concatenate(rot, inc);
		SetLocalRotation(rot);
	}

	// �J�����̈ʒu�̓I�[�i�[�̈ʒu
	Vector3 cameraPos = mPosition;

	// �s�b�`���x�Ɋ�Â��ăs�b�`���X�V
	mPitch += mPitchSpeed * Time::gUnscaledDeltaTime;
	// �N�����v�s�b�`��[-max, +max]�ɐ���
	mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);
	// �I�[�i�[�̉E�x�N�g�������Ƃ���s�b�`��]��\���l�������쐬
	Quaternion q(GetRight(), mPitch);

	// ���L�҂��s�b�`�N�H�[�^�j�I���őO���ɉ�]
	Vector3 viewForward = Vector3::Transform(
		GetForward(), q);
	// �����̑O��100���j�b�g�̃^�[�Q�b�g�ʒu�B
	Vector3 target = cameraPos + viewForward * 100.0f;
	// �s�b�`�N�H�[�^�j�I������]�B
	Vector3 up = Vector3::Transform(Vector3::UnitY, q);

	// �}�g���b�N�X���쐬���A�r���[�Ƃ��Đݒ�
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	WindowRenderProperty::SetViewEye(cameraPos);
	WindowRenderProperty::SetViewTarget(target);
	WindowRenderProperty::SetViewUp(up);
	mViewMatrix = view;

	//X�AZ�ړ�����
	// �O��ړ��A���E�ړ�
	if (!Math::NearZero(mForwardSpeed) || !Math::NearZero(mStrafeSpeed))
	{
		Vector3 pos = mLocalPosition;
		pos += viewForward * mForwardSpeed * Time::gUnscaledDeltaTime;
		pos += GetRight() * mStrafeSpeed * Time::gUnscaledDeltaTime;
		SetLocalPosition(pos);
		ComputeWorldTransform();
	}
	
}

void SceneEditorCamera::ProcessInput(const struct InputState& keyState)
{
	//�}�E�X����
	mYawSpeed = 0;
	mPitchSpeed = 0;
	mIsViewDirty = false;
	// SceneView�p�l���Ƀ}�E�X������Ă��Ȃ��ꍇ�A�������Ȃ�
	if (!GUIWinMain::GetSceneViewPanel()->IsMouseHovered()) { return; }
	// �E�N���b�N�������ꂽ�u�ԁA���΃��[�h�ɐ؂�ւ�
	if (keyState.Mouse.GetButtonDown(SDL_BUTTON_RIGHT))
	{
		InputSystem::RelativeMouseMode();
	}
	// �E�N���b�N��������Ă��Ȃ��ꍇ�A�������Ȃ�
	if(keyState.Mouse.GetButton(SDL_BUTTON_RIGHT))
	{
		mIsViewDirty = true;
		//SDL�Ń}�E�X�̈ړ����l���擾
		float x, y;
		SDL_GetRelativeMouseState(&x, &y);
		//�}�E�X�ł̍ő�ړ����l��ݒ�

		// �ő呬�x�ł̉�]/�b
		const float maxAngularSpeed = Math::Pi * MAX_YAW_SPEED;
		float angularSpeed = 0.0f;
		if (x != 0)
		{
			// [-1.0, 1.0]�ɕϊ�����
			angularSpeed = static_cast<float>(x) / mMouseSensitivityX;
			// ��]/�b�Ŋ|����
			angularSpeed *= maxAngularSpeed;
		}
		mYawSpeed = angularSpeed;

		// �������v�Z����
		const float maxPitchSpeed = Math::Pi * MAX_YAW_SPEED;
		float pitchSpeed = 0.0f;
		if (y != 0)
		{
			// [-1.0, 1.0]�ɕϊ�����
			pitchSpeed = static_cast<float>(y) / mMouseSensitivityY;
			pitchSpeed *= maxPitchSpeed;
		}
		mPitchSpeed = pitchSpeed;
	}
	if (keyState.Mouse.GetButtonUp(SDL_BUTTON_RIGHT))
	{
		InputSystem::AbsoluteMouseMode();
	}

	Vector2 wheel = keyState.Mouse.GetScrollWheel();

	mSpeed += wheel.y;
	if (mSpeed >= mMaxSpeed)
	{
		mSpeed = mMaxSpeed;
	}
	else if (mSpeed <= mMinSpeed)
	{
		mSpeed = mMinSpeed;
	}

	// �L�[�{�[�h����
	float forwardSpeed = 0.0f;
	float strafeSpeed = 0.0f;
	// wasd movement
	if (keyState.Keyboard.GetKey(SDL_SCANCODE_W))
	{
		forwardSpeed += mSpeed;
	}
	if (keyState.Keyboard.GetKey(SDL_SCANCODE_S))
	{
		forwardSpeed -= mSpeed;
	}
	if (keyState.Keyboard.GetKey(SDL_SCANCODE_A))
	{
		strafeSpeed -= mSpeed;
	}
	if (keyState.Keyboard.GetKey(SDL_SCANCODE_D))
	{
		strafeSpeed += mSpeed;
	}
	mForwardSpeed = forwardSpeed;
	mStrafeSpeed = strafeSpeed;
}