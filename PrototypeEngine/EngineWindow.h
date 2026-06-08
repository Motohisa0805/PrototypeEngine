#pragma once
#include "Typedefs.h"
#include "GameWinMain.h"
#include "PhysWorld.h"

enum EngineState
{
	Run,
	End
};

//前方宣言
class Renderer;
class GameWinMain;
class SceneEditorCamera;
class ScriptHotReloadManager;

class EngineWindow
{
private:
	//エンジン用のRendererを用意	
	static Renderer*						mRenderer;

	static PhysWorld*						mPhysWorld;

	//ゲーム内のウィンドウ
	GameWinMain*							mGameWindow;

	static EngineState						mEngineState;


	std::unique_ptr<ScriptHotReloadManager> mHotReloadManager;
public:
								EngineWindow();
								~EngineWindow();

	bool						EngineInitialize();

	void						EngineProcessInput();

	void						EngineRunLoop();
	void						EngineRender();

	void						EngineShutdown();

	static Renderer*			GetRenderer() { return mRenderer; }

	static PhysWorld*			GetPhysWorld() { return mPhysWorld; }

	static EngineState			GetEngineState() { return mEngineState; }
	static void					SetEngineState(EngineState state) { mEngineState = state; }
};

