#pragma once
#include "SDOpenLib.h"
#include "GameWinMain.h"
#include "GUIWinMain.h"
#include "SceneManager.h"

enum EngineState
{
	Run,
	End
};

class EngineWindow
{
private:
	//エンジン用のRendererを用意	
	static class Renderer* mRenderer;

	//ゲーム内のウィンドウ
	class GameWinMain* mGameWindow;

	static EngineState mEngineState;

public:
					EngineWindow();
					~EngineWindow();

	bool			EngineInitialize();

	void			EngineProcessInput();

	void			EngineRunLoop();
	void			EngineRender();

	void			EngineUnloadData();
	void			EngineShutdown();

	static class Renderer* GetRenderer() { return mRenderer; }

	static EngineState GetEngineState() { return mEngineState; }
	static void SetEngineState(EngineState state) { mEngineState = state; }
};

