#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//ゲームのシーン内のオブジェクトを描画する予定のクラス
//本格的な描画処理は未実装
class HierarchyPanel : public GUIPanel
{
private:
public:
	const char* GetName()override { return "Hierarchy"; }
	HierarchyPanel(class Renderer* renderer);

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	void		Draw(float width, float height, ImTextureRef ref = nullptr)override;
};

