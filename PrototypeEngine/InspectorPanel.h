#pragma once
#include "EditorWindow.h"

//オブジェクトなどを選択した時にそのオブジェクトの情報を描画するクラス
//本格的な描画処理は未実装
class InspectorPanel : public EditorWindow
{
public:
    struct ModelImportSettings
    {
        float sGlobalScale = 1.0f;
        bool  sFlipUVs     = true;
        bool  sRecalculateNormals = false;
        bool  sImportMaterials    = true;
        bool  sImportAnimations   = true;
    };

private:

	//Transformプロパティを描画するためのヘルパー関数を宣言
	void		DrawTransformProperties(Entity* transform);

	void		DrawComponentProperties(class Component* comp, const PropertyInfo& prop);
public:

	InspectorPanel(Renderer* renderer);
	~InspectorPanel();

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	void		Draw(float width, float height)override;

	void		ActorInspection(Entity* selectedActor);

	void		FileInspection(const filesystem::path& selectedFilePath);

	static void	ComponentSelectorDraw(Entity* selectedActor);
};

