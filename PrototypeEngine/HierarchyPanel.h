#pragma once
#include "GUIPanel.h"
#include "GUIWinMain.h"

class HierarchyPanel : public GUIPanel
{
private:
	class Renderer* mRenderer;
public:
	HierarchyPanel(class Renderer* renderer);

	void Draw(float width, float height);

	const char* GetName()override { return "Hierarchy"; }
};

