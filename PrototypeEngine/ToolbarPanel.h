#pragma once
#include "GUIPanel.h"

//�Q�[���̍Đ��A��~�A�ꎞ��~�A�R�}����Ȃǉ�ʂ̏㕔����GUI�̕`�揈���N���X
class ToolbarPanel : public GUIPanel
{
private:

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

