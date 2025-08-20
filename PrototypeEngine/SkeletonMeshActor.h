#pragma once
#include "Actor.h"
#include "Mesh.h"
#include "SkeletalMeshRenderer.h"
#include "Animator.h"
#include "BoxCollider.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//3Dモデルを読み込むクラス
//スケルトン有りの3Dモデルを
//読み込みだけのクラス
//ActorObjectを継承しているので1オブジェクトとしてカウント
class SkeletonMeshActor : public ActorObject
{
private:
	class SkeletalMeshRenderer* mSkeletonMesh;

	class Animator*				mAnimator;

	string						mAnimatorName;
public:
	SkeletonMeshActor();
	~SkeletonMeshActor();

	void Load(string filePath);

	class SkeletalMeshRenderer* GetSkeletalMeshRenderer() { return mSkeletonMesh; }

	class Animator* GetAnimator() { return mAnimator; }

	void SetAnimatorName(string name);
};

