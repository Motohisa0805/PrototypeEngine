#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//�G�f�B�^�[�V�[���̕`�揈���N���X
//
class SceneViewPanel : public GUIPanel
{
private:
public:
	SceneViewPanel(class Renderer* renderer);

	bool		MouseHoveredDisble()override;

	void		Draw(float width, float height, ImTextureRef ref);

	const char* GetName()override { return "Scene"; }
};

