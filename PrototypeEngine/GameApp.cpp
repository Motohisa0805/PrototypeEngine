#include "GameApp.h"

#include "TitleScene.h"
#include "DebugScene01.h"
#include "DebugScene02.h"


GameApp::GameApp(GameWinMain* main) 
	:mWinMain(main)
{

}

bool GameApp::Initialize()
{
	//�Q�[���̏�Ԃ�ݒ�
	GameStateClass::SetGameState(GameState::GamePlay);
	return true;
}

bool GameApp::ProcessInput()
{
	const InputState& state = InputSystem::GetState();
	//���͍X�V
	SceneManager::GetNowScene()->InputUpdate(state);
	return true;
}

bool GameApp::LoadUpdate()
{
	//���[�h�t���O��true�Ȃ�
	if (SceneManager::IsLoading())
	{
		SceneManager::ChangeScene();
	}
	return true;
}

bool GameApp::Update()
{
	SceneManager::GetNowScene()->FixedUpdate();
	SceneManager::GetNowScene()->Update();
	return true;
}

bool GameApp::Release()
{
	//�Q�[���V�[���̉��
	SceneManager::GetNowScene()->UnloadData();
	//���̓V�X�e���̃V���b�g�_�E��
	InputSystem::Shutdown();
	return true;
}