#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"
#include "Component.h"

//オブジェクトなどを選択した時にそのオブジェクトの情報を描画するクラス
//本格的な描画処理は未実装
class InspectorPanel : public GUIPanel
{
private:
	//Transformプロパティを描画するためのヘルパー関数を宣言
	void		DrawTransformProperties(class Transform* transform);

	void		DrawComponentProperties(class Component* comp, const PropertyInfo& prop);
public:
	const char* GetName()override { return "SelectItem"; }

	InspectorPanel(class Renderer* renderer);
	~InspectorPanel();

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	void		Draw(float width, float height, ImTextureRef ref = nullptr)override;

};

