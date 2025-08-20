#pragma once
#include "SDOpenLib.h"

#include "GUIWinMain.h"

enum GameState
{
	GamePlay,
	TimeStop,
	GameEnd
};

class GameStateClass 
{
public:
	static void			SetGameState(GameState state) { mGameState = state; }
	static GameState	mGameState;
	static bool			mGameEventFrag;
	static bool			mDebugFrag;
};
//�Q�[���̕`�揈���Ǘ��N���X
//���Renderern�Ȃǂ̉�ʂ̍\�����Ǘ����Ă���N���X
class GameWinMain
{
private:
	static class Renderer*		mRenderer;

	//�Q�[��������
	class GameApp*		mGameApp;
	//�Q�[�����̑S�Ă̕`����s���N���X
	void				Render();
	//�V�[���J�ڎ��̉������
	void				UnloadData();

public:
						GameWinMain();
						~GameWinMain();
	//������
	bool				Initialize();
	//�Q�[������
	void				RunLoop();
	//�Q�[���I������
	void				Shutdown();
	//Renderer�̎擾
	static class Renderer*		GetRenderer() { return mRenderer; }
};