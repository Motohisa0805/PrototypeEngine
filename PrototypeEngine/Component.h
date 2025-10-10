#pragma once
#include <cstdint>
#include "Typedefs.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//�R���|�[�l���g�̊��N���X
// Unity�̃R���|�[�l���g�ɋ߂��N���X
class Component
{
protected:
	// �R���|�[�l���g���̃I�u�W�F�N�g
	class ActorObject*	mOwner;
	class BaseScene*	mGame;
	// �R���|�[�l���g�̏������X�V����
	int					mUpdateOrder;

	string				mName;
public:
	// �R���X�g���N�^
	// �i�X�V�������Ⴂ�قǁA�R���|�[�l���g�������X�V�����j
						Component(class ActorObject* owner, int updateOrder = 100);
	// �f�X�g���N�^�[
	virtual				~Component();
	virtual void		FixedUpdate(float deltaTime);
	// �f���^���Ԃł��̃R���|�[�l���g���X�V���Ă�������
	virtual void		Update(float deltaTime);
	// ���̃R���|�[�l���g�̓��͂���������
	virtual void		ProcessInput(const struct InputState& keyState) {}
	// ���E�̕ω��ɂ���ČĂяo�����
	virtual void		OnUpdateWorldTransform() { }

	class ActorObject*	GetOwner() { return mOwner; }
	int					GetUpdateOrder() const { return mUpdateOrder; }

	string				GetName()const { return mName; }

	// JSON�ɕϊ����郁�\�b�h
	virtual void		Serialize(json& j) const;
	// JSON���畜�����郁�\�b�h
	virtual void		Deserialize(const json& j);

	virtual void		DrawGUI(){}
};