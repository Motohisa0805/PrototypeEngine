#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//エディターシーンの描画処理クラス
//
class SceneViewPanel : public GUIPanel
{
private:
public:
	SceneViewPanel(class Renderer* renderer);

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	bool		MouseHoveredDisble()override;

	void		Draw(float width, float height, ImTextureRef ref)override;

	const char* GetName()override { return "Scene"; }
};

