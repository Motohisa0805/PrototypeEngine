#pragma once
#include "Math.h"
#include "BaseScene.h"
#include "Component.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//�O���錾
class Matrix4;
//�I�u�W�F�N�g�̍��W�A��]�A�X�P�[�����O�̏����Ǘ�
//Unity��Transform�ɋ߂��N���X
class Transform
{
protected:

	//���[�J�����W�A��]�A�X�P�[�����O
	Vector3								mLocalPosition;
	Vector3								mPositionOffset;
	Quaternion							mLocalRotation;
	Vector3								mLocalScale;

	//�v�Z���ʂ��L���b�V��
	//������mWorldTransform���番�����ꂽ�l�ł���A���ڕҏW���Ă炾��
	Vector3								mPosition;
	Quaternion							mRotation;
	Vector3								mScale;

	Matrix4								mLocalTransform;
	Matrix4								mWorldTransform;

	bool								mIsDirty;

	//�e�I�u�W�F�N�g
	Transform*							mParentActor;
	//�q�I�u�W�F�N�g�̔z��
	vector<Transform*>					mChildActor;

	//X�AY�AZ���̉�]��
	float								mRotationAmountX;
	float								mRotationAmountY;
	float								mRotationAmountZ;


	vector<Component*>					mComponents;
public:
										Transform();

	virtual								~Transform();

	virtual const Matrix4&				GetWorldTransform() const { return mWorldTransform; }

	virtual void						SetWorldTransform(Matrix4 mat) { mWorldTransform = mat; }

	virtual const Matrix4&				GetLocalTransform() const { return mLocalTransform; }

	virtual Vector3						GetForward() const { return Vector3::Transform(Vector3::UnitZ, mLocalRotation); }

	virtual Vector3						GetRight() const { return Vector3::Transform(Vector3::UnitX, mLocalRotation); }

	virtual Vector3						GetUp() const { return Vector3::Transform(Vector3::UnitY, mLocalRotation); }

	virtual void						RotateToNewForward(const Vector3& forward);

	virtual void						LookAt(const Vector3& targetPosition);

	// Getters/setters
	virtual const Vector3&				GetPosition() 
	{
		ComputeWorldTransform(); // �K�v�Ȃ�X�V
		return mPosition;
	}
	virtual void						SetPosition(const Vector3& pos);
	virtual const Quaternion&			GetRotation() 
	{
		ComputeWorldTransform(); // �K�v�Ȃ�X�V
		return mRotation;
	}
	virtual void						SetRotation(const Quaternion& rotation);
	virtual const Vector3&				GetScale()  
	{
		ComputeWorldTransform(); // �K�v�Ȃ�X�V
		return mScale;
	}
	// 1 Ver
	virtual void						SetScale(Vector3 scale);
	// 2 Ver
	virtual void						SetScale(float scale)
	{
		mScale = Vector3(scale, scale, scale);
		SetDirty();
	}

	// Position��Getters/setters
	virtual const Vector3&				GetLocalPosition() const { return mLocalPosition; }
	virtual void						SetLocalPosition(const Vector3& pos)
	{
		mLocalPosition = pos;
		SetDirty();
	}
	// Scale��Getters/setters
	virtual Vector3						GetLocalScale() const { return mLocalScale; }
	// 1 Ver
	virtual void						SetLocalScale(Vector3 scale)
	{
		mLocalScale = scale;
		SetDirty();
	}
	// 2 Ver
	virtual void						SetLocalScale(float scale)
	{
		mLocalScale = Vector3(scale, scale, scale);
		SetDirty();
	}
	// Rotation��Getters/setters
	virtual const Quaternion&			GetLocalRotation() const { return mLocalRotation; }

	virtual void						SetLocalRotation(const Quaternion& rotation)
	{
		mLocalRotation = rotation;
		SetDirty();
	}


	//���ʂ̉�]�ʂ�Getters/setters
	virtual float						GetRotationAmountX() { return mRotationAmountX; }
	virtual float						GetRotationAmountY() { return mRotationAmountY; }
	virtual float						GetRotationAmountZ() { return mRotationAmountZ; }
	virtual void						SetRotationAmountX(float rot) { mRotationAmountX = rot; }
	virtual void						SetRotationAmountY(float rot) { mRotationAmountY = rot; }
	virtual void						SetRotationAmountZ(float rot) { mRotationAmountZ = rot; }

	//���[���h���W�̍X�V		
	virtual void						ComputeWorldTransform();

	virtual void						LocalBonePositionUpdateActor(Matrix4 boneMatrix, const Matrix4& parentActor);

	//***�q�I�u�W�F�N�g�֌W�̏���***
	virtual Transform*					GetParentActor() { return mParentActor; }

	virtual const Transform*			GetChildActor(Transform* actor);

	//�q�I�u�W�F�N�g��ǉ�
	virtual void						AddChildActor(Transform* actor);

	virtual void						RemoveChildActor(Transform* actor);


	virtual void						AddParentActor(Transform* parent);

	virtual void						RemoveParentActor();

	// Add/remove components
	virtual void						AddComponent(Component* component);
	virtual void						RemoveComponent(Component* component);
	const vector<Component*>&			GetComponents()const { return mComponents; }

	//�q�I�u�W�F�N�g�̍��W�X�V
	virtual void						SetDirty();


	// JSON�ɕϊ����郁�\�b�h
	virtual void						Serialize(json& j) const;
	// JSON���畜�����郁�\�b�h
	virtual void						Deserialize(const json& j);
};

