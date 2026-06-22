#pragma once
#include "EditorWindow.h"

//オブジェクトなどを選択した時にそのオブジェクトの情報を描画するクラス
//本格的な描画処理は未実装
class InspectorPanel : public EditorWindow
{
private:

	//Transformプロパティを描画するためのヘルパー関数を宣言
	void		DrawTransformProperties(Entity* transform);

	void		DrawComponentProperties(class Component* comp, const PropertyInfo& prop);
public:

	InspectorPanel(Renderer* renderer);
	~InspectorPanel();

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	void		Draw(float width, float height)override;

	static void	ComponentSelectorDraw(Entity* selectedActor);
};

