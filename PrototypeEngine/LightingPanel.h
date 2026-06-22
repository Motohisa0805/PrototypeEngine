#pragma once
#include "EditorWindow.h"

class LightingPanel : public EditorWindow
{
public:
	LightingPanel(Renderer* renderer);
	~LightingPanel();
	void Initialize(float width, float height, ImTextureRef ref = nullptr)override;
	void Draw(float width, float height)override;
};

