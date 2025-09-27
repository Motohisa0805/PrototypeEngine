#pragma once
#include "GUIPanel.h"

class GUIMainMenu : public GUIPanel
{
private:

public:
	GUIMainMenu(class Renderer* renderer);
	~GUIMainMenu();
	void Initialize(float width, float height, ImTextureRef ref = nullptr)override;
	void Draw(float width, float height, ImTextureRef ref = nullptr)override;
	const char* GetName()override { return "MainMenu"; }
};

