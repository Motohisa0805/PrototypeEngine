#include "SceneManager.h"
#include "BaseScene.h"
#include "SceneSerializer.h"
#include "EditorSettingsManager.h"

BaseScene* SceneManager::mNowScene = nullptr;

bool SceneManager::loading = false;

int SceneManager::mNowSceneIndex = 0;

// ���[�h�\�񂳂ꂽ�t�@�C���p�X��ێ�����ϐ�
std::string SceneManager::mNextSceneFilePath = "";

string SceneManager::mDefaultSceneFilePath = "Assets/Scenes/TestScene01.json";

bool SceneManager::InitializeScenes()
{
	//1.�N���V�[���̃p�X���擾
	string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();

	//2.�V�[���I�u�W�F�N�g�̍쐬�ƃ��[�h
	if (!startupScenePath.empty())
	{
		//�p�X���L���Ȃ�A���̃t�@�C�����烍�[�h�����݂�
		mNowScene = SceneSerializer::LoadScene(startupScenePath);

	}

	//�x�[�X�ɍŏ��̓��I�V�[����ݒ�(���EditorScene���쐬)
	if (mNowScene == nullptr)
	{
		mNowScene = new EditorScene();
	}
	//�V�[���̏�����
	if (!mNowScene->Initialize())
	{
		return false;
	}
	//Renderer�Ɍ��݂̃V�[����ݒ�
	EngineWindow::GetRenderer()->SetBaseScene(mNowScene);
	return true;
}

void SceneManager::LoadSceneFromFile(const string& filePath)
{
	// �����̃V�[�����X�g�ɒǉ�����̂ł͂Ȃ��A�ꎞ�I�Ƀt�@�C���p�X��ێ����A
	// ChangeScene() �̃^�C�~���O�ŏ��������s���܂��B
	mNextSceneFilePath = filePath;
	loading = true; // ChangeScene()���Ăяo�����悤�Ƀt���O�𗧂Ă�
}

void SceneManager::ReleaseAllScenes()
{

	mNowScene->UnloadData();
	delete mNowScene;
	mNowScene = nullptr;
}

void SceneManager::ChangeScene()
{
	//�t�@�C���̃p�X���Z�b�g����Ă���ꍇ
	if (!mNextSceneFilePath.empty())
	{
		if (mNowScene)
		{
			EngineWindow::GetRenderer()->UnloadData();
			mNowScene->UnloadData();

			//�ŏd�v�F�Â��V�[���̃��������
			delete mNowScene;
			mNowScene = nullptr;
		}

		mNowScene = SceneSerializer::LoadScene(mNextSceneFilePath);
		mNowScene->Initialize();
		EngineWindow::GetRenderer()->SetBaseScene(mNowScene);
		//...(�㑱�̏���)...
	}
	loading = false;
}

void SceneManager::SetCurrentEditorSceneFilePath(const string& path)
{
	mDefaultSceneFilePath = path;
}
