#pragma once
#include "StandardLibrary.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//�V�[���̊Ǘ��A���[�h�������s���N���X
class SceneManager
{
private:
	//���[�h�Ώۂ̃V�[�����X�g
	static std::unordered_map<int, class BaseScene*>	mScenes;
	//���݂̃V�[��
	static class BaseScene*								mNowScene;
	//���[�h�t���O
	static bool											loading;
	//���݂̃V�[���̐��l
	static int											mNowSceneIndex;


	static class TitleScene*							mTitleScene;
	//�V�[��01
	static class DebugScene01*							mDebugScene01;
	//�V�[��02
	static class DebugScene02*							mDebugScene02;
public:
	//�V�[���̏���������
	static bool											InitializeScenes();

	//�V�[���̃��[�h����
	static void											LoadScene(int index);
	//�V�[�����X�g�ɒǉ�
	static void											AddSceneList(class BaseScene* scene);
	//�V�[���̍폜
	static void											ReleaseAllScenes();
	//���݂̃V�[���̎擾
	static class BaseScene*								GetNowScene() { return mNowScene; }
	//���[�h�t���O�̎擾
	static bool											IsLoading() { return loading; }
	//���[�h�t���O������
	static void											DisabledLoading() { loading = false; }
	//���݂̃V�[���̐��l��Getter
	static int											GetNowSceneIndex() { return mNowSceneIndex; }
};

