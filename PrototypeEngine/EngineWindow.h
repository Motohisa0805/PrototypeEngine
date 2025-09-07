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

//�O���錾
class Renderer;
class GameWinMain;
class SceneEditorCamera;

class EngineWindow
{
private:
	//�G���W���p��Renderer��p��	
	static Renderer*			mRenderer;

	//�Q�[�����̃E�B���h�E
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

