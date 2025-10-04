#pragma once
#include "GUIPanel.h"

class GUIMainMenu : public GUIPanel
{
private:

public:
	const char* GetName()override { return "MainMenu"; }
	GUIMainMenu(class Renderer* renderer);
	~GUIMainMenu();
	//GUIの初期化
	void Initialize(float width, float height, ImTextureRef ref = nullptr)override;
	//GUIの描画
	void Draw(float width, float height, ImTextureRef ref = nullptr)override;
	//ファイルメニューの描画
	void FileMenuDraw();
	//表示メニューの描画
	void ViewMenuDraw();
};

