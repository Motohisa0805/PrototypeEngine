#include "DebugScene01.h"
#include "GameFunctions.h"

DebugScene01::DebugScene01()
	: mFPSActor(nullptr)
	, mStages(nullptr)
	, mGameCanvas00(nullptr)
	, mPauseMenu(nullptr)
{
}

bool DebugScene01::Initialize()
{
	BaseScene::Initialize();
	// Load English text
	//LoadText("Assets/English.gptext");


	// �|�C���g���C�g���b�V�������[�h����
	//mWinMain->GetRenderer()->SetPointLightMesh(mWinMain->GetRenderer()->GetMesh("PointLight.gpmesh"));

	
	// BGM�X�^�[�g
	mMusicEvent = mAudioSystem->PlayEvent("event:/Music3");
	// BGM�ꎞ��~
	mMusicEvent.Pause();

	InputSystem::SetGameMouseMode(InputSystem::RelativeMouse);

	// �f�o�b�O�p�̃X�e�[�W�ǉ�
	mStages = new Stages00();

	mStages->Initialize();

	// �v���C���[����
	mFPSActor = new FPSActor();
	mFPSActor->SetLocalPosition(Vector3(0.0f,1.0f,0.0f));

	mPlayer = mFPSActor;

	


	// �Q�[������UI����
	mGameCanvas00 = new FPSCanvas();

	LoadSkyBoxTexture("SkyBox02.png");
	return true;
}

bool DebugScene01::InputUpdate(const InputState& state)
{
	BaseScene::InputUpdate(state);
	if (state.Keyboard.GetKeyDown(KEY_O))
	{
		mMusicEvent.Pause();
	}
	else if (state.Keyboard.GetKeyDown(KEY_P))
	{
		mMusicEvent.Restart();
	}

	if (!mCanvasStack.empty())
	{
		mCanvasStack.back()->ProcessInput(state);
	}


	return true;
}

bool DebugScene01::Update()
{

	BaseScene::Update();
	return true;
}
