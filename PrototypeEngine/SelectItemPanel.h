#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//オブジェクトなどを選択した時にそのオブジェクトの情報を描画するクラス
//本格的な描画処理は未実装
class SelectItemPanel : public GUIPanel
{
private:
	class Renderer* mRenderer;
public:
	SelectItemPanel(class Renderer* renderer);

	void Draw(float width, float height);

	const char* GetName()override { return "SelectItem"; }
};

