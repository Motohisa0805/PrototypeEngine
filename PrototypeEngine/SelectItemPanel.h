#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//�I�u�W�F�N�g�Ȃǂ�I���������ɂ��̃I�u�W�F�N�g�̏���`�悷��N���X
//�{�i�I�ȕ`�揈���͖�����
class SelectItemPanel : public GUIPanel
{
private:
	class Renderer* mRenderer;
public:
	SelectItemPanel(class Renderer* renderer);

	void Draw(float width, float height);

	const char* GetName()override { return "SelectItem"; }
};

