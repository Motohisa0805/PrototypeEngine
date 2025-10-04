#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//GUI�̃Q�[���p�l���̏������܂Ƃ߂��N���X
class GameViewPanel : public GUIPanel
{
private:
public:
	const char* GetName()override { return "Game"; }
	GameViewPanel(class Renderer* renderer);

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	void		Draw(float width,float height,ImTextureRef ref);
};

