#pragma once
#include "EditorWindow.h"
#include "Renderer.h"
#include "EngineWindow.h"


class LightingPanel : public EditorWindow
{
private:
	Vector2 mPanelSize;
public:
	LightingPanel(class Renderer* renderer);
	~LightingPanel();
	void Initialize(float width, float height, ImTextureRef ref = nullptr)override;
	void Draw(float width, float height)override;
};

