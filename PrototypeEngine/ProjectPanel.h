#pragma once
#include "GUIPanel.h"
#include "GUIWinMain.h"

class ProjectPanel : public GUIPanel
{
private:
	class Renderer* mRenderer;
public:
	ProjectPanel(class Renderer* renderer);

	void Draw(float width, float height);

	const char* GetName()override { return "Project"; }
};

