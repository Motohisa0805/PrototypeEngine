
#include "GameWinMain.h"
#include "EngineWindow.h"
#include <SDL3/SDL.h>

#define DISABLE_DEBUG_NEW // 一時的に無効にして
#include "Debug_memory.h"
#undef DISABLE_DEBUG_NEW   // すぐに解除


//TODO : プロジェクトを実行している場所
//構成マネージャーがDebugなら
#ifdef _CONSOLE
int main(int argc, char* argv[])
{
	// メモリリーク検出を有効にする
	EnableMemoryLeakCheck();
	// エンジンの初期化
	EngineWindow engine;
	bool success = engine.EngineInitialize();
	if (success)
	{
		engine.EngineRunLoop();
	}
	engine.EngineShutdown();
	/*
	//エンジンの初期化
	GameWinMain game;
	bool success = game.Initialize();
	if (success)
	{
		game.RunLoop();
	}
	game.Shutdown();
	*/
	return 0;
}
// プロジェクトの実行場所がReleaseなら
#elif defined(_WINDOWS)
#define SDL_MAIN_HANDLED
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	GameWinMain game;
	bool success = game.Initialize();
	if (success)
	{
		game.RunLoop();
	}
	game.Shutdown();
	return 0;
}
#endif