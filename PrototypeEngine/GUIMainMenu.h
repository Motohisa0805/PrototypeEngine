#pragma once
#include "GUIPanel.h"

class GUIMainMenu : public GUIPanel
{
private:

public:
	const char* GetName()override { return "MainMenu"; }
	GUIMainMenu(class Renderer* renderer);
	~GUIMainMenu();
	//GUI�̏�����
	void Initialize(float width, float height, ImTextureRef ref = nullptr)override;
	//GUI�̕`��
	void Draw(float width, float height, ImTextureRef ref = nullptr)override;
	//�t�@�C�����j���[�̕`��
	void FileMenuDraw();
	//�\�����j���[�̕`��
	void ViewMenuDraw();
};

