#include "GameWinMain.h"
#include <windows.h>

//Game�p�̎��s�֐�
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
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