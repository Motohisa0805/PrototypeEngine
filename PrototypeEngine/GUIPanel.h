#pragma once
#include "Texture.h"
#include "Renderer.h"

//GUI�̃p�l���̌p�����N���X
//���O�A�}�E�X�J�[�\�����p�l�����ɂ��邩�ǂ����Ȃǂ̏������s���Ă��܂��B
class GUIPanel
{
protected:
	bool isMouseHovered;
private:

public:
	GUIPanel();

	virtual bool		MouseHoveredDisble();

	bool				WindowHoveredConfirmation();

	bool				IsMouseHovered() const { return isMouseHovered; }
	virtual const char* GetName() { return "BasePanel"; }
};

