#include "GameApp.h"

#include "TitleScene.h"
#include "DebugScene01.h"
#include "DebugScene02.h"


BaseScene* GameApp::mActiveScene = nullptr;

GameApp::GameApp(GameWinMain* main) 
	:mWinMain(main)
{

}

bool GameApp::Initialize()
{
	//�V�[������
	mTitleScene = new TitleScene();
	mDebugScene01 = new DebugScene01();
	mDebugScene02 = new DebugScene02();
	//�V�[�������X�g�ɒǉ�
	SceneManager::AddSceneList(mTitleScene);
	SceneManager::AddSceneList(mDebugScene01);
	SceneManager::AddSceneList(mDebugScene02);
	//�x�[�X�ɍŏ��̃V�[����ݒ�
	mActiveScene = mTitleScene;
	//�V�[���̏�����
	if (!mActiveScene->Initialize())
	{
		return false;
	}
	//�Q�[���̏�Ԃ�ݒ�
	GameStateClass::SetGameState(GameState::GamePlay);
	//Renderer�Ɍ��݂̃V�[����ݒ�
	GameWinMain::GetRenderer()->SetBaseScene(mActiveScene);
	return true;
}

bool GameApp::ProcessInput()
{
	const InputState& state = InputSystem::GetState();
	InputSystem::Update();

	//���͑���
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		// ImGui�p�̃C�x���g����
		ImGui_ImplSDL3_ProcessEvent(&event);
		switch (event.type)
		{
			//���s���I�������true
		case SDL_EVENT_QUIT:
			GameStateClass::SetGameState(GameState::GameEnd);
			break;
		}
	}

	//���͍X�V
	mActiveScene->InputUpdate(state);
	InputSystem::PrepareForUpdate();
	return true;
}

bool GameApp::LoadUpdate()
{
	//���[�h�t���O��true�Ȃ�
	if (SceneManager::IsLoading())
	{
		//���݂̃V�[���̃I�u�W�F�N�g�A�摜�Ȃǂ��A�����[�h
		mActiveScene->UnloadData();
		//Renderer�̂��̂��A�����[�h
		mWinMain->GetRenderer()->UnloadData();
		//�V�[����ύX
		mActiveScene = SceneManager::GetNowScene();
		GameStateClass::SetGameState(GameState::GamePlay);
		//static���ύX
		//�V�����V�[���̏�����
		mActiveScene->Initialize();
		//Renderer�̃V�[�����ύX
		mWinMain->GetRenderer()->SetBaseScene(mActiveScene);
		//���[�h�t���O������
		SceneManager::DisabledLoading();
	}
	return true;
}

bool GameApp::Update()
{
	mActiveScene->FixedUpdate();
	mActiveScene->Update();
	return true;
}

bool GameApp::Release()
{
	mActiveScene->UnloadData();
	SceneManager::ReleaseAllScenes();
	InputSystem::Shutdown();
	return true;
}