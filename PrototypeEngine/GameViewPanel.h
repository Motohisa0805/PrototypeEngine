#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//GUIのゲームパネルの処理をまとめたクラス
class GameViewPanel : public GUIPanel
{
private:
public:
	GameViewPanel(class Renderer* renderer);

	void		Draw(float width,float height,ImTextureRef ref);

	const char* GetName()override { return "Game"; }
};

