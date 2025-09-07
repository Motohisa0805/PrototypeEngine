#pragma once
#include "Math.h"
#include "Texture.h"
#include "Shader.h"
#include "GameWinMain.h"
#include "Font.h"
#include "Image.h"
#include "Text.h"

#include "Button.h"
/*
* ===�G���W����������/Engine internal processing===
*/

class Shader;

//�|�[�Y�A�_�C�A���O�Ȃǂ�UI�̊��N���X
// Unity��Canvas�ɋ߂��N���X
class Canvas
{
public:
	// UI���A�N�e�B�u���Ǘ�����^�O
	enum UIState
	{
		EActive,
		EClosing,
		EDestroy
	};
protected:
	// �摜��`�悷��֐�
	void					DrawTexture(Shader* shader,Texture* texture,
		const Vector2& offset = Vector2::Zero,
		Vector3 scale = Vector3(1.0f, 1.0f, 1.0f), float angle = 0);
	class BaseScene* mGame;

	Image*		mTitleImage;
	Text*		mTitleFont;
	Image*		mTitle;
	Image*		mBackground;

	Texture*	mButtonOn;
	Texture*	mButtonOff;

	// �ʒu��ݒ肷��
	Vector2					mTitlePos;
	Vector2					mNextButtonPos;
	Vector2					mBGPos;

	// ���
	UIState					mState;
	// �{�^���̃��X�g
	vector<Button*>			mButtons;

	vector<Image*>			mImages;
public:
							Canvas();
	virtual					~Canvas();
	// UIScreen�̃T�u�N���X�͂������I�[�o�[���C�h�ł��܂�
	virtual void			Update(float deltaTime);
	virtual void			Draw(class Shader* shader);
	virtual void			ProcessInput(const struct InputState& keys);

	// ��Ԃ���ɐݒ�
	virtual void					Close();
	// UI��ʂ̏�Ԃ��擾����
	UIState					GetState() const { return mState; }
	void					SetState(UIState state);
	// �^�C�g���e�L�X�g��ύX����
	void					SetTitle(const string& text,
							const Vector3& color = Color::White,
							int pointSize = 40);
	// ���̉�ʂɃ{�^����ǉ�����֐�
	void					AddButton(const string& name, std::function<void()> onClick);
	class Button*			CreateButton(const string& name,const Vector2& pos, std::function<void()> onClick);
	class Button*			CreateButton(const char8_t* name,const Vector2& pos, std::function<void()> onClick);

	void					AddChildUIImage(Image* image);
};