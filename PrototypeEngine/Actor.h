#pragma once
#include "BaseScene.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "VertexArray.h"
#include "Shader.h"
#include "ComponentFactory.h"

/*
* ===�G���W����������/Engine internal processing===
*/

enum ActorTag
{
	None,
	Player,
	Ground,
	Enemy,
	Recovery
};

//�O���錾
class Rigidbody;
//�S3D���f���̊��N���X
class ActorObject : public Transform
{
public:
	//�I�u�W�F�N�g�̏��
	enum State
	{
		EActive,
		EPaused,
		EDead
	};
protected:
	string						mName;
	// Actor's state
	State						mState;

	//�I�u�W�F�N�g�̃^�O
	ActorTag					mActorTag = ActorTag::None;

	BaseScene*					mGame;

	Rigidbody*					mRigidbody;

	Collider*					mCollider;
public:
	//�R���X�g���N�^
								ActorObject();
								ActorObject(BaseScene* scene);
	//�f�X�g���N�^
	virtual						~ActorObject();
	// �Q�[�������萔�ŌĂяo�����X�V�֐��i�I�[�o�[���C�h�s�j
	void						FixedUpdate(float deltaTime);
	// Actor�ɐڑ����ꂽ���ׂẴR���|�[�l���g���X�V���܂��i�I�[�o�[���C�h�s�j
	void						FixedUpdateComponents(float deltaTime);
	// �C�ӂ�Actor�ŗL�̍X�V�R�[�h�i�㏑���\�j
	virtual void				FixedUpdateActor(float deltaTime);
	// �Q�[������Ăяo�����X�V�֐��i�I�[�o�[���C�h�s�j
	void						Update(float deltaTime);
	// Actor�ɐڑ����ꂽ���ׂẴR���|�[�l���g���X�V���܂��i�I�[�o�[���C�h�s�j
	void						UpdateComponents(float deltaTime);
	// �C�ӂ�Actor�ŗL�̍X�V�R�[�h�i�㏑���\�j
	virtual void				UpdateActor(float deltaTime);
	// �Q�[������Ăяo���ꂽProcessInput�֐��i�I�[�o�[���C�h�ł��܂���j
	void						ProcessInput(const struct InputState& keyState);
	// �C�ӂ�Actor���L�̓��̓R�[�h�i�㏑���\�j
	virtual void				ActorInput(const struct InputState& keyState);

	template<typename T>
	T*							GetComponent() const;

	// Getters/setters
	State						GetState() const { return mState; }

	void						SetState(State state) { mState = state; }

	virtual ActorTag			GetActorTag() { return mActorTag; }

	void						SetActorTag(ActorTag tag) { mActorTag = tag; }
	
	BaseScene*					GetGame() { return mGame; }
	Rigidbody*					GetRigidbody() { return mRigidbody; }

	Collider*					GetCollider() { return mCollider; }


	//�e�̃A�N�^�[��Getter
	//�����������ɌĂяo�����֐�
	virtual void				OnCollisionEnter(ActorObject* target){}
	//�������Ă��鎞�ɌĂяo�����֐�
	virtual void				OnCollisionStay(ActorObject* target){}
	//������I��������ɌĂяo�����֐�
	virtual void				OnCollisionExit(ActorObject* target){}

	string						GetName() const { return mName; }

	void 						SetName(const string& name) { mName = name; }


	// JSON�ɕϊ����郁�\�b�h
	void						Serialize(json& j) const override;
	// JSON���畜�����郁�\�b�h
	void						Deserialize(const json& j)override;
	//�R���|�[�l���g���ǉ����ꂽ��ɌĂ΂��ʒm�֐�
	virtual void				OnComponentAdded(Component* newComp);
};

template<typename T>
inline T* ActorObject::GetComponent() const
{
	for (auto component : mComponents)
	{
		if (T* casted = dynamic_cast<T*>(component))
		{
			return casted;
		}
	}
	return nullptr; // �w�肵���^�̃R���|�[�l���g��������Ȃ������ꍇ
}
