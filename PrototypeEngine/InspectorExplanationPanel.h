#pragma once
#include "EditorWindow.h"


class InspectorExplanationPanel : public EditorWindow
{
private:
	Vector2 mPanelSize;
public:
	InspectorExplanationPanel(class Renderer* renderer);
	~InspectorExplanationPanel();
	void Initialize(float width, float height, ImTextureRef ref = nullptr)override;
	void ResetWindowPos(float width, float height)override;
	void Draw(float width, float height)override;
};

