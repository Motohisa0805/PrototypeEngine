#pragma once
#include "Actor.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "Mesh.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//3D���f����ǂݍ��ރN���X
//�X�P���g��������3D���f����
//�ǂݍ��݂����̃N���X
//ActorObject���p�����Ă���̂�1�I�u�W�F�N�g�Ƃ��ăJ�E���g
class MeshActor : public ActorObject
{
private:
	MeshRenderer*				mMeshRenderer;

	BoxCollider*				mBoxCollider;

	SphereCollider*				mSphereCollider;

	CapsuleCollider*			mCapsuleCollider;

public:
	//�R���X�g���N�^
							MeshActor();
							~MeshActor();

	void					Load(string filePath);

	void					SetColliderMode(bool active);

	void					SetStaticMode(bool active);

	void					AddBoxCollider();

	void					AddSphereCollider();

	void					AddCapsuleCollider();

	MeshRenderer*			GetMeshRenderer() { return mMeshRenderer; }

	BoxCollider*			GetBoxCollider() { return mBoxCollider; }

	SphereCollider*			GetSphereCollider() { return mSphereCollider; }

	CapsuleCollider*		GetCapsuleCollider() { return mCapsuleCollider; }

	//MeshRenderer����AABB���擾�������ȗ������֐�
	const AABB				GetBoxAABB();
	//MeshRenderer����OBB���擾�������ȗ������֐�
	const OBB				GetBoxOBB();
};