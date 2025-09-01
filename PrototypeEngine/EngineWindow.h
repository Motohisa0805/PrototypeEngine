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

class EngineWindow
{
private:
	//�G���W���p��Renderer��p��	
	static class Renderer* mRenderer;

	//�Q�[�����̃E�B���h�E
	class GameWinMain* mGameWindow;

	static EngineState mEngineState;

	static class SceneEditorCamera* mSceneEditorCamera;

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

	static class SceneEditorCamera* GetSceneEditorCamera() { return mSceneEditorCamera; }
};

