#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

class SelectItemPanel : public GUIPanel
{
private:
	class Renderer* mRenderer;
public:
	SelectItemPanel(class Renderer* renderer);

	void Draw(float width, float height);

	const char* GetName()override { return "SelectItem"; }
};

