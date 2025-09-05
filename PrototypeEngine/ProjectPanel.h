#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//プロジェクトのファイルパスを描画するクラス
//本格的な描画処理は未実装
class ProjectPanel : public GUIPanel
{
private:
	class Renderer* mRenderer;
public:
	ProjectPanel(class Renderer* renderer);

	void		Draw(float width, float height);

	const char* GetName()override { return "Project"; }
};

