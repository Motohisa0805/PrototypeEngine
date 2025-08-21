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
//ゲームの描画処理管理クラス
//主にRenderernなどの画面の構成を管理しているクラス
class GameWinMain
{
private:

	//ゲーム内処理
	class GameApp*		mGameApp;

	//シーン遷移時の解放処理
	void				UnloadData();

public:
						GameWinMain();
						~GameWinMain();
	//初期化
	bool				Initialize();
	void				InputUpdate();
	//ゲーム処理
	void				RunLoop();
	void				GameRunLoop();
	//ゲーム終了処理
	void				Shutdown();
};