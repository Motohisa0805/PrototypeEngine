#pragma once
#include "Actor.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//�X�P���^�����b�V���̃{�[��1��1�̏����Ǘ�����BoneActor
//��Ɏq�I�u�W�F�N�g�̈ʒu�Ǘ����s���Ă��܂��B
class BoneActor : public ActorObject
{
private:
	string			boneName;

	int				boneIndex;

	Matrix4			mBoneMatrix;
	//�e�̃{�[���̐��l
	int				mParentIndex;
public:
	BoneActor();
	void			ComputeWorldTransform()override;

	void			SetBoneName(string name) { boneName = name; mName = name; }

	void			SetBoneIndex(int index) { boneIndex = index; }

	void			SetBoneMatrix(Matrix4 mat) { mBoneMatrix = mat; }

	const int		GetParentIndex()const  { return mParentIndex; }

	void			SetParentIndex(int index) { mParentIndex = index; }
};

