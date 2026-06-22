#pragma once
#include "EditorWindow.h"

class AboutEnginePanel : public EditorWindow
{
private:
	Vector2 mPanelSize;
public:
	AboutEnginePanel(Renderer* renderer);
	~AboutEnginePanel();
	void Initialize(float width, float height, ImTextureRef ref = nullptr)override;
	void ResetWindowPos(float width, float height)override;
	void Draw(float width, float height)override;
};