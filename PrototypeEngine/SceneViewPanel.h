#pragma once
#include "GUIEditorManager.h"
#include "EditorWindow.h"

//エディターシーンの描画処理クラス
//
class SceneViewPanel : public EditorWindow
{
private:
public:
	const char* GetName()override { return "Scene"; }

	SceneViewPanel(class Renderer* renderer);

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	bool		MouseHoveredDisble()override;

	void		Draw(float width, float height, ImTextureRef ref)override;
};

