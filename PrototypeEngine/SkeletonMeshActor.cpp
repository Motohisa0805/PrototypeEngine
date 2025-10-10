#include "SkeletonMeshActor.h"

SkeletonMeshActor::SkeletonMeshActor()
	: ActorObject()
	, mSkeletonMesh(nullptr)
	, mAnimator(nullptr)
{
	mName = "SkeletonMeshActor";

	mSkeletonMesh = new SkeletalMeshRenderer(this);
	mAnimator = new Animator(this);

	mSkeletonMesh->SetAnimator(mAnimator);

}

SkeletonMeshActor::~SkeletonMeshActor()
{
}

void SkeletonMeshActor::Load(string filePath)
{
	mSkeletonMesh->LoadSkeletonMesh(filePath, this);

	mAnimator->SetSkeleton(mSkeletonMesh->GetSkeleton());
}

void SkeletonMeshActor::SetAnimatorName(string name)
{
	mAnimatorName = name;
	//GetGame()->GetAnimator(mAnimatorName, mAnimator);
}
