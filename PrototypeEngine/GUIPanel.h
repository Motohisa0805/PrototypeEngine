#pragma once
#include "Texture.h"
#include "Renderer.h"

//�O���錾
class Renderer;
//GUI�̃p�l���̌p�����N���X
//���O�A�}�E�X�J�[�\�����p�l�����ɂ��邩�ǂ����Ȃǂ̏������s���Ă��܂��B
class GUIPanel
{
protected:
	Renderer*			mRenderer;
	bool				isMouseHovered;
private:

public:
	GUIPanel(Renderer* renderer);

	virtual bool		MouseHoveredDisble();

	bool				WindowHoveredConfirmation();

	bool				IsMouseHovered() const { return isMouseHovered; }
	virtual const char* GetName() { return "BasePanel"; }
};

