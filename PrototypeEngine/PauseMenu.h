#pragma once
#include "Canvas.h"

//エンジンでは派生クラスとして拡張性がなくなるの廃止予定

class GameDialogBox;
//書籍元のファイル
//ポーズUIのクラス
class PauseMenu : public Canvas
{
private:
	GameDialogBox*	mDialogBox;
public:
					PauseMenu();
					~PauseMenu();
	void			ProcessInput(const struct InputState& keys)override;
};