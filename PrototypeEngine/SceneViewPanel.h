#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

class SceneViewPanel : public GUIPanel
{
private:
	class Renderer* mRenderer;
public:
	SceneViewPanel(class Renderer* renderer);

	void Draw(float width, float height, ImTextureRef ref);

	const char* GetName()override { return "Scene"; }
};

