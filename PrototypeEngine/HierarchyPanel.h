#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//�Q�[���̃V�[�����̃I�u�W�F�N�g��`�悷��\��̃N���X
//�{�i�I�ȕ`�揈���͖�����
class HierarchyPanel : public GUIPanel
{
private:
	class Renderer* mRenderer;
public:
	HierarchyPanel(class Renderer* renderer);

	void		Draw(float width, float height);

	const char* GetName()override { return "Hierarchy"; }
};

