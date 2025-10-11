#pragma once
#include "StandardLibrary.h"
#include "SceneSerializer.h"
/*
* ===�G���W����������/Engine internal processing===
*/

//�O���錾
class BaseScene;

//�V�[���̊Ǘ��A���[�h�������s���N���X
class SceneManager
{
private:

	//���݂̃V�[��
	static BaseScene*									mNowScene;
	//���[�h�t���O
	static bool											loading;
	//���݂̃V�[���̐��l
	static int											mNowSceneIndex;
	static string										mNextSceneFilePath;
public:
	//�V�[���̏���������
	static bool											InitializeScenes();

	//�V�[���̃��[�h����
	static void											LoadSceneFromFile(const string& filePath);
	// ���ݕҏW��/���s���̃V�[�����A�t�@�C�����烍�[�h���ꂽ���̂��������t���O�i�C�Ӂj
	static bool											mIsFileLoadedScene;
	//�V�[���̍폜
	static void											ReleaseAllScenes();
	//���݂̃V�[���̎擾
	static BaseScene*									GetNowScene() { return mNowScene; }
	static void											SetNowScene(BaseScene* scene) { mNowScene = scene; }
	static void											ChangeScene();
	static void											PlayEndInitilaizeScene();
	//���[�h�t���O�̎擾
	static bool											IsLoading() { return loading; }
	//���[�h�t���O������
	static void											DisabledLoading() { loading = false; }
	//���݂̃V�[���̐��l��Getter
	static int											GetNowSceneIndex() { return mNowSceneIndex; }
	//�G���W���N�����Ƀ��[�h���ׂ��V�[���t�@�C���̃p�X
	static string										mDefaultSceneFilePath;
	//���݃G�f�B�^�ŊJ���Ă���V�[���̃t�@�C���p�X��ݒ肷��֐�
	static void											SetCurrentEditorSceneFilePath(const string& path);
};

