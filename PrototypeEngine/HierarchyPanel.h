#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//�Q�[���̃V�[�����̃I�u�W�F�N�g��`�悷��\��̃N���X
//�{�i�I�ȕ`�揈���͖�����
class HierarchyPanel : public GUIPanel
{
private:
public:
	const char* GetName()override { return "Hierarchy"; }
	HierarchyPanel(class Renderer* renderer);

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	void		Draw(float width, float height, ImTextureRef ref = nullptr)override;
};

