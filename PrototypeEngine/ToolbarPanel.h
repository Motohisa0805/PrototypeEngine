#pragma once
#include "GUIPanel.h"

class ToolbarPanel : public GUIPanel
{
private:
	class Renderer* mRenderer;

	Texture* mPlayButtonTexture;
	Texture* mPauseButtonTexture;
	Texture* mStopButtonTexture;
	Texture* mFrameByFrameButtonTexture;
public:
	ToolbarPanel(class Renderer* renderer);
	~ToolbarPanel();
	void Initialize();
	void Draw(float width, float height);
	const char* GetName()override { return "Toolbar"; }
};

