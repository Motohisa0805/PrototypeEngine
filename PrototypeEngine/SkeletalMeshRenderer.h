#pragma once
#include "MeshRenderer.h"
#include "Animator.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class Animator;
class Skeleton;

//スケルタルメッシュの描画を行うクラス
//MeshRendererと同様スケルタルメッシュを読み込んで使用する
class SkeletalMeshRenderer : public MeshRenderer
{
protected:
	Animator*						mAnimator;

	Skeleton*						mSkeleton;
public:
									SkeletalMeshRenderer(ActorObject* owner);
									~SkeletalMeshRenderer();
	// スケルタルモデルの描画
	void							Draw(Shader* shader) override;

	void							DrawForShadowMap(Shader* shader)override;

	void							Update(float deltaTime) override;

	void							LoadSkeletonMesh(const string& fileName,ActorObject* actor);
	
	// Setters
	void							SetSkeleton(Skeleton* sk, ActorObject* actor);


	void							SetAnimator(Animator* animator);

	Skeleton*						GetSkeleton() { return mSkeleton; }

	void							Serialize(json& j) const override;
	void							Deserialize(const json& j)override;

	void							DrawGUI()override;
};