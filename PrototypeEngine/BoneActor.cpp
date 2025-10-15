#include "BoneActor.h"

BoneActor::BoneActor()
	: ActorObject()
{
}

void BoneActor::ComputeWorldTransform()
{
	//�X�V�t���O��true�Ȃ�
	if (mIsDirty)
	{
		mIsDirty = false;

		mLocalTransform = Matrix4::CreateScale(mLocalScale);
		mLocalTransform *= Matrix4::CreateFromQuaternion(mLocalRotation);
		mLocalTransform *= Matrix4::CreateTranslation(mLocalPosition);

		//�e��������
		if (mParentActor) 
		{
			mParentActor->ComputeWorldTransform();
			mWorldTransform = mLocalTransform * mParentActor->GetWorldTransform();
		}
		//���Ȃ�������
		else 
		{
			//�{�[���̃}�g���b�N�X�Ƀ��f�����g�̃}�g���b�N�X����Z
			mWorldTransform = mLocalTransform;
		}

		// Inform components world transform updated
		for (auto comp : mComponents)
		{
			comp->OnUpdateWorldTransform();
		}

		//�q�I�u�W�F�N�g�̍��W�v�Z
		for (auto child : mChildActor)
		{
			child->SetDirty();
		}
	}
}
