#include "GameWinMain.h"
#include <windows.h>

//GameópÇÃé¿çsä÷êî
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