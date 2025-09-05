#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//ゲームのシーン内のオブジェクトを描画する予定のクラス
//本格的な描画処理は未実装
class HierarchyPanel : public GUIPanel
{
private:
	class Renderer* mRenderer;
public:
	HierarchyPanel(class Renderer* renderer);

	void		Draw(float width, float height);

	const char* GetName()override { return "Hierarchy"; }
};

