#pragma once
#include "SDL3.h"
#include "Renderer.h"

//�O���錾
class GameWinMain;

//�Q�[���S�̂̏������s���N���X
//�����ɃQ�[���ɕK�v�ȏ����������Ă����z��Ő��삵�܂����B
class GameApp
{
private:
	//�`��Ǘ��N���X
	GameWinMain*				mWinMain;
public:
								GameApp(class GameWinMain* main);
	//������
	bool						Initialize();
	//���͏���
	bool						ProcessInput();
	//�V�[���̃��[�h����
	bool						LoadUpdate();
	//�X�V����
	bool						Update();
	//���
	bool						Release();
};