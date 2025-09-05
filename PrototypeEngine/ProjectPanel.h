#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//�v���W�F�N�g�̃t�@�C���p�X��`�悷��N���X
//�{�i�I�ȕ`�揈���͖�����
class ProjectPanel : public GUIPanel
{
private:
	class Renderer* mRenderer;
public:
	ProjectPanel(class Renderer* renderer);

	void		Draw(float width, float height);

	const char* GetName()override { return "Project"; }
};

