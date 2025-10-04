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
	float				mWidthPos;
	float				mHeightPos;
	float				mWidthSize;
	float				mHeightSize;
	//GUI�̃��C�A�E�g�������ʒu�ɖ߂��t���O
	bool				isResetLayout;
public:
	virtual const char* GetName() { return "BasePanel"; }
	GUIPanel(Renderer* renderer);

	virtual void		Initialize(float width, float height, ImTextureRef ref = nullptr);

	virtual bool		MouseHoveredDisble();

	virtual void		ResetWindowPos(float width, float height);

	bool				WindowHoveredConfirmation();

	virtual void		Draw(float width, float height, ImTextureRef ref = nullptr);

	virtual void        GUIPanelMenu();

	bool				IsMouseHovered() const { return isMouseHovered; }

	float				GetWindowSizeWidth();
	float				GetWindowSizeHeight();
	//��ʃT�C�Y�����ɃA�X�y�N�g����v�Z
	ImVec2				GetAspectRatio();

	void				EnableResetLayout() { isResetLayout = true; }
};

