#pragma once
#include "StandardLibrary.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//�O���錾
class BaseScene;

//�V�[���̊Ǘ��A���[�h�������s���N���X
class SceneManager
{
private:
	//���[�h�Ώۂ̃V�[�����X�g
	static std::unordered_map<int, BaseScene*>			mScenes;
	//���݂̃V�[��
	static BaseScene*									mNowScene;
	static BaseScene*									mNextScene;
	//���[�h�t���O
	static bool											loading;
	//���݂̃V�[���̐��l
	static int											mNowSceneIndex;
public:
	//�V�[���̏���������
	static bool											InitializeScenes();

	//�V�[���̃��[�h����
	static void											LoadScene(int index);
	//�V�[�����X�g�ɒǉ�
	static void											AddSceneList(BaseScene* scene);
	//�V�[���̍폜
	static void											ReleaseAllScenes();
	//���݂̃V�[���̎擾
	static BaseScene*									GetNowScene() { return mNowScene; }
	static void											ChangeScene();
	//���[�h�t���O�̎擾
	static bool											IsLoading() { return loading; }
	//���[�h�t���O������
	static void											DisabledLoading() { loading = false; }
	//���݂̃V�[���̐��l��Getter
	static int											GetNowSceneIndex() { return mNowSceneIndex; }
};

