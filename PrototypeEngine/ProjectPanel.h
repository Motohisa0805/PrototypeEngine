#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

class ProjectPanel : public GUIPanel
{
private:
	class Renderer* mRenderer;
public:
	ProjectPanel(class Renderer* renderer);

	void Draw(float width, float height);

	const char* GetName()override { return "Project"; }
};

