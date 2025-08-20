#pragma once
#include "SDOpenLib.h"
#include "GameWinMain.h"
#include "GUIWinMain.h"
#include "SceneManager.h"

enum EngineState
{
	None,
	Run,
	End
};

class EngineWindow
{
private:
	//エンジン用のRendererを用意	
	class Renderer* mRenderer;

	//ゲーム内のウィンドウ
	class GameWinMain* mGameWindow;

	static EngineState mEngineState;

public:
					EngineWindow();
					~EngineWindow();

	bool			EngineInitialize();

	void			EngineRunLoop();
	void			EngineRender();

	void			EngineUnloadData();
	void			EngineShutdown();

	class Renderer* GetRenderer() { return mRenderer; }

	static EngineState GetEngineState() { return mEngineState; }
	static void SetEngineState(EngineState state) { mEngineState = state; }
};

