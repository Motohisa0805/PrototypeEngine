#pragma once
#include "Typedefs.h"
#include "Assimp.h"
#include "SDOpenLib.h"
#include "StandardLibrary.h"

#include "GameWinMain.h"
#include "GUIWinMain.h"
#include "SceneManager.h"

enum EngineState
{
	Run,
	End
};

//前方宣言
class Renderer;
class GameWinMain;
class SceneEditorCamera;

class EngineWindow
{
private:
	//エンジン用のRendererを用意	
	static Renderer*			mRenderer;

	//ゲーム内のウィンドウ
	GameWinMain*				mGameWindow;

	static EngineState			mEngineState;

	static SceneEditorCamera*	mSceneEditorCamera;

public:
					EngineWindow();
					~EngineWindow();

	bool			EngineInitialize();

	void			EngineProcessInput();

	void			EngineRunLoop();
	void			EngineRender();

	void			EngineUnloadData();
	void			EngineShutdown();

	static Renderer*			GetRenderer() { return mRenderer; }

	static EngineState			GetEngineState() { return mEngineState; }
	static void					SetEngineState(EngineState state) { mEngineState = state; }

	static SceneEditorCamera*	GetSceneEditorCamera() { return mSceneEditorCamera; }
};

