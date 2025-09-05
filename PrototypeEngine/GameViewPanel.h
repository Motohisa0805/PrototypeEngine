#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//GUI�̃Q�[���p�l���̏������܂Ƃ߂��N���X
class GameViewPanel : public GUIPanel
{
private:
	class Renderer* mRenderer;
public:
	GameViewPanel(class Renderer* renderer);

	void		Draw(float width,float height,ImTextureRef ref);

	const char* GetName()override { return "Game"; }
};

