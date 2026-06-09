#pragma once
#include "EditorWindow.h"

class HierarchyExplanationPanel : public EditorWindow
{
private:
	Vector2 mPanelSize;
public:
	HierarchyExplanationPanel(class Renderer* renderer);
	~HierarchyExplanationPanel();
	void Initialize(float width, float height, ImTextureRef ref = nullptr)override;
	void ResetWindowPos(float width, float height)override;
	void Draw(float width, float height)override;
};

