#pragma once
#include "Canvas.h"
#include "DialogBox.h"
#include <SDL3/SDL.h>

//書籍元のファイル
//ポーズUIのクラス
class PauseMenu : public Canvas
{
private:
	class GameDialogBox* mDialogBox;
public:
			PauseMenu();
			~PauseMenu();
	void	ProcessInput(const struct InputState& keys)override;
};