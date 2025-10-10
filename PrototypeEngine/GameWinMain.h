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

class GameApp;

//�Q�[���̕`�揈���Ǘ��N���X
//���Renderern�Ȃǂ̉�ʂ̍\�����Ǘ����Ă���N���X
class GameWinMain
{
private:

	//�Q�[��������
	GameApp*			mGameApp;

	//�V�[���J�ڎ��̉������
	void				UnloadData();

public:
						GameWinMain();
						~GameWinMain();
	//������
	bool				Initialize();
	void				InputUpdate();
	//�Q�[������
	void				RunLoop();
	void				GameRunLoop();
	void				LoadGame_Engine();
	//�Q�[���I������
	void				Shutdown();
};