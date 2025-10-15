#include "BoneActor.h"

BoneActor::BoneActor()
	: ActorObject()
{
}

void BoneActor::ComputeWorldTransform()
{
	//更新フラグがtrueなら
	if (mIsDirty)
	{
		mIsDirty = false;

		mLocalTransform = Matrix4::CreateScale(mLocalScale);
		mLocalTransform *= Matrix4::CreateFromQuaternion(mLocalRotation);
		mLocalTransform *= Matrix4::CreateTranslation(mLocalPosition);

		//親がいたら
		if (mParentActor) 
		{
			mParentActor->ComputeWorldTransform();
			mWorldTransform = mLocalTransform * mParentActor->GetWorldTransform();
		}
		//いなかったら
		else 
		{
			//ボーンのマトリックスにモデル自身のマトリックスを乗算
			mWorldTransform = mLocalTransform;
		}

		// Inform components world transform updated
		for (auto comp : mComponents)
		{
			comp->OnUpdateWorldTransform();
		}

		//子オブジェクトの座標計算
		for (auto child : mChildActor)
		{
			child->SetDirty();
		}
	}
}
