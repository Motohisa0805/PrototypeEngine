#pragma once
#include "GameApp.h"

enum GameState
{
	GamePlay,
	TimeStop,
	GameEnd
};

class GameStateClass 
{
public:
	static void			SetGameState(GameState state) { gGameState = state; }
	static GameState	gGameState;
	static bool			gGameEventFrag;

	//エディターシーン用のフラグ
	static bool			gDebugGridFrag;

	static bool			gShadowFrag;
	//ゲームシーン用のフラグ
	static bool			gDebugStatesFrag;
};

class GameApp;

//ゲームの描画処理管理クラス
//主にRenderernなどの画面の構成を管理しているクラス
class GameWinMain
{
private:

	//ゲーム内処理
	GameApp*			mGameApp;

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
	void				LoadGame_Engine();
	//ゲーム終了処理
	void				Shutdown();
};