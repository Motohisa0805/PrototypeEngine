#pragma once
#include "EditorWindow.h"

class ProjectExplanationPanel : public EditorWindow
{
private:
	Vector2 mPanelSize;
public:
	ProjectExplanationPanel(Renderer* renderer);
	~ProjectExplanationPanel();
	void Initialize(float width, float height, ImTextureRef ref = nullptr)override;
	void ResetWindowPos(float width, float height)override;
	void Draw(float width, float height)override;
};

