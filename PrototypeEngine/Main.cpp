
#include "GameWinMain.h"
#include "EngineWindow.h"
#include <SDL3/SDL.h>

#define DISABLE_DEBUG_NEW // �ꎞ�I�ɖ����ɂ���
#include "Debug_memory.h"
#undef DISABLE_DEBUG_NEW   // �����ɉ���


//TODO : �v���W�F�N�g�����s���Ă���ꏊ
//�\���}�l�[�W���[��Debug�Ȃ�
#ifdef _CONSOLE
int main(int argc, char* argv[])
{
	// ���������[�N���o��L���ɂ���
	EnableMemoryLeakCheck();
	// �G���W���̏�����
	EngineWindow engine;
	bool success = engine.EngineInitialize();
	if (success)
	{
		engine.EngineRunLoop();
	}
	engine.EngineShutdown();
	/*
	//�G���W���̏�����
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
// �v���W�F�N�g�̎��s�ꏊ��Release�Ȃ�
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