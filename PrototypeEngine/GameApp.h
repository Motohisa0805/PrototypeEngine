#pragma once
#include "SDL3.h"
#include "Renderer.h"

//�Q�[���S�̂̏������s���N���X
//�����ɃQ�[���ɕK�v�ȏ����������Ă����z��Ő��삵�܂����B
class GameApp
{
private:
	//�`��Ǘ��N���X
	class GameWinMain*			mWinMain;
public:
								GameApp(class GameWinMain* main);
	//������
	bool						Initialize();
	//���͏���
	bool						ProcessInput();
	bool						ProcessInput2();
	//�V�[���̃��[�h����
	bool						LoadUpdate();
	//�X�V����
	bool						Update();
	//���
	bool						Release();
};