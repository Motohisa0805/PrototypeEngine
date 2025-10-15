#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//オブジェクトなどを選択した時にそのオブジェクトの情報を描画するクラス
//本格的な描画処理は未実装
class InspectorPanel : public GUIPanel
{
private:
public:
	const char* GetName()override { return "SelectItem"; }

	InspectorPanel(class Renderer* renderer);
	~InspectorPanel();

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	void		Draw(float width, float height, ImTextureRef ref = nullptr)override;
};

