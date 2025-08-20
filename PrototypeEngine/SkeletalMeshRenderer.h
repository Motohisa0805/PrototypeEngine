#pragma once
#include "MeshRenderer.h"
#include "Animator.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//スケルタルメッシュの描画を行うクラス
//MeshRendererと同様スケルタルメッシュを読み込んで使用する
class SkeletalMeshRenderer : public MeshRenderer
{
public:
									SkeletalMeshRenderer(class ActorObject* owner);
									~SkeletalMeshRenderer();
	// スケルタルモデルの描画
	void							Draw(class Shader* shader) override;

	void							DrawForShadowMap(class Shader* shader)override;

	void							Update(float deltaTime) override;

	void							LoadSkeletonMesh(const string& fileName,class ActorObject* actor);
	
	// Setters
	void							SetSkeleton(class Skeleton* sk, class ActorObject* actor) 
	{
		mSkeleton = sk; 
		mSkeleton->SetParentActor(actor);
	}


	void							SetAnimator(class Animator* animator);

	class Skeleton*					GetSkeleton() { return mSkeleton; }

protected:

	class Animator*					mAnimator;

	class Skeleton*					mSkeleton;
};