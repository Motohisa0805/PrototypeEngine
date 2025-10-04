#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//�I�u�W�F�N�g�Ȃǂ�I���������ɂ��̃I�u�W�F�N�g�̏���`�悷��N���X
//�{�i�I�ȕ`�揈���͖�����
class SelectItemPanel : public GUIPanel
{
private:
public:
	const char* GetName()override { return "SelectItem"; }

	SelectItemPanel(class Renderer* renderer);

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	void Draw(float width, float height, ImTextureRef ref = nullptr)override;
};

