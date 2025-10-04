#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//�G�f�B�^�[�V�[���̕`�揈���N���X
//
class SceneViewPanel : public GUIPanel
{
private:
public:
	const char* GetName()override { return "Scene"; }

	SceneViewPanel(class Renderer* renderer);

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	bool		MouseHoveredDisble()override;

	void		Draw(float width, float height, ImTextureRef ref)override;
};

