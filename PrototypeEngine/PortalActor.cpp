#include "PortalActor.h"

PortalActor::PortalActor()
	: ActorObject()
	, mMeshActor(nullptr)
	, mBoxCollider(nullptr)
	, mMeshAlpha(0.5f)
{
	mName = "PortalActor";

	mMeshActor = new MeshActor();
	mMeshActor->Load("Portal.fbx");
	mMeshActor->GetMeshRenderer()->SetMaterialAlpha(mMeshAlpha);
	AddChildActor(mMeshActor);

	mBoxCollider = new BoxCollider(this);
	mBoxCollider->SetObjectOBB(mMeshActor->GetBoxOBB());
	mBoxCollider->SetCollider(false);
}

PortalActor::~PortalActor()
{
}

void PortalActor::RelativeMouseMode(bool relative)
{
	if (SceneManager::IsLoading()) { return; }
	if (relative)
	{
		SDL_SetWindowRelativeMouseMode(EngineWindow::GetRenderer()->GetWindow(), true);
		// Make an initial call to get relative to clear out
		SDL_GetRelativeMouseState(nullptr, nullptr);
	}
	else
	{
		SDL_SetWindowRelativeMouseMode(EngineWindow::GetRenderer()->GetWindow(), false);
	}
}

void PortalActor::UpdateActor(float deltaTime)
{
	/*
	//Y���ɉ�]��������
	float y = GetRotationAmountY();
	y += 5.0f * deltaTime;
	Quaternion q = Quaternion(Vector3::UnitY, y);
	SetLocalRotation(q);
	SetRotationAmountY(y);
	*/
}

void PortalActor::OnCollisionEnter(ActorObject* other)
{
	if(other->GetActorTag() != ActorTag::Player)
	{
		return; // �v���C���[�ȊO�͖���
	}
	//�V�[����J�ڂ��邩�̃_�C�A���O��\��
	GameDialogBox* dialogBox = new GameDialogBox(u8"�ʃV�[���Ɉړ����܂����H",
		[this]() {
			int s = SceneManager::GetNowSceneIndex() == 1 ? 2 : 1;
			/*SceneManager::LoadScene(s)*/;
		},
		[this]() {
			// �L�����Z�����̏���
			GameStateClass::SetGameState(GameState::TimeStop);
			Time::gTimeScale = 0;
			RelativeMouseMode(false);
		},
		[this]() {
			// �L�����Z�����̏���
			GameStateClass::SetGameState(GameState::GamePlay);
			Time::gTimeScale = 1;
			RelativeMouseMode(true);
		});
}
