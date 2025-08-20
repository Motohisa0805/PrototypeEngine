#include "Animator.h"

Animator::Animator(ActorObject* owner)
	: Component(owner)
	, mSkeleton(nullptr)
	, mAnimation(nullptr)
	, mBlendAnimation(nullptr)
	, mAnimTime(0.0f)
	, mBlendAnimTime(0.0f)
	, mAnimPlayRate(1.0f)
	, mBlendElapsed(0.1f)
	, mBlending(false)
{
}

Animator::~Animator()
{
	for(int i = 0; i < mAnimations.size(); i++)
	{
		if (mAnimations[i])
		{
			delete mAnimations[i];
			mAnimations[i] = nullptr;
		}
	}
}

/// <summary>
/// アニメーションを読み込む処理
/// </summary>
/// <param name="fileName">
/// アニメーションのファイル名
/// </param>
/// <param name="animLoop">
/// アニメーションのループ設定
/// </param>
/// <returns></returns>
bool Animator::Load(const string& fileName,bool animLoop, bool rootMotion)
{
	string path = File_P::AnimationFilePath + fileName;
	Animation* anim = new Animation(mSkeleton);
	anim->SetLoop(animLoop);
	anim->SetRootMotion(rootMotion);

	/*
	if (anim->LoadFromBinary(fileName))
	{
		mAnimations.push_back(anim);
		return true;
	}
	else
	*/
	if (anim->Load(path))
	{
		mAnimations.push_back(anim);
		return true;
	}
	else
	{
		delete anim;
		anim = nullptr;
	}
	return false;
}

void Animator::Update(float deltaTime)
{
	if (mAnimation && mSkeleton)
	{
		mAnimTime += deltaTime * mAnimPlayRate;
		if (mAnimation->IsLoop())
		{
			if (mAnimTime > mAnimation->GetDuration())
			{
				mAnimTime = 0.0f;
			}
		}
		else
		{
			if (mAnimTime > mAnimation->GetDuration())
			{
				mAnimTime = mAnimation->GetDuration();
				if (!mAnimation->IsAnimationEnd())
				{
					mAnimation->SetIsAnimationEnd(true);
				}
			}
		}
		// Recompute matrix palette
		ComputeMatrixPalette();
	}

	if (mBlending)
	{
		mBlendAnimTime += deltaTime * mAnimPlayRate;

		float t = Math::Clamp(mBlendAnimTime / mBlendElapsed, 0.0f, 1.0f);

		BlendComputeMatrixPalette();

		if (mBlendAnimTime >= mBlendElapsed)
		{
			mAnimTime = mBlendAnimTime;
			mAnimation = mBlendAnimation;
			mBlendAnimation = nullptr;
			mBlending = false;
		}
	}
}

void Animator::SetSkeleton(Skeleton* skeleton)
{
	if (skeleton == nullptr)
	{
		Debug::ErrorLog("The project is ending because there are no Skeleton.");
		return;
	}
	mSkeleton = skeleton;
}

float Animator::PlayAnimation(Animation* anim)
{
	if (mAnimation == anim) { return 0.0f; }
	mAnimation = anim;
	mAnimTime = 0.0f;

	mAnimation->SetIsAnimationEnd(false);

	if (!mAnimation) { return 0.0f; }

	ComputeMatrixPalette();

	return mAnimation->GetDuration();
}

float Animator::PlayBlendAnimation(Animation* anim)
{
	if (mAnimation == anim || mBlendAnimation == anim) { return 0.0f; }

	mBlendAnimation = anim;
	mBlendAnimTime = 0.0f;

	mBlendAnimation->SetIsAnimationEnd(false);
	mAnimation->SetIsAnimationEnd(false);
	mBlending = true;

	return mAnimation->GetDuration();
}

void Animator::ComputeMatrixPalette()
{
	const vector<Matrix4>& globalInvBindPoses = mSkeleton->GetGlobalInvBindPoses();
	vector<Matrix4> currentPoses;
	mAnimation->GetGlobalPoseAtTime(currentPoses, mSkeleton, mAnimTime);
	mSkeleton->SetGlobalCurrentPoses(currentPoses);
	// Setup the palette for each bone
	for (size_t i = 0; i < mSkeleton->GetNumBones(); i++)
	{
		Matrix4 pose = currentPoses[i];
		// Global inverse bind pose matrix times current pose matrix
		mPalette.mEntry[i] = globalInvBindPoses[i] * pose;
		Matrix4 transform;
		if (mOwner->GetParentActor() == nullptr)
		{
			transform = pose;
		}
		else
		{
			transform = pose * mOwner->GetWorldTransform();
		}
		mSkeleton->GetBoneActor()[i]->SetLocalScale(transform.GetScale());
		mSkeleton->GetBoneActor()[i]->SetLocalRotation(transform.GetRotation());
		mSkeleton->GetBoneActor()[i]->SetLocalPosition(transform.GetTranslation());
	}
}

void Animator::BlendComputeMatrixPalette()
{
	const vector<Matrix4>& globalInvBindPoses = mSkeleton->GetGlobalInvBindPoses();
	vector<Matrix4> nowPose;
	vector<Matrix4> nextPose;
	vector<Matrix4> goalPose;

	// 経過時間に対する補間率
	float t = Math::Clamp(mBlendAnimTime / mBlendElapsed, 0.0f, 1.0f);

	// アニメーションタイムを使ってそれぞれのポーズを取得
	mAnimation->GetGlobalPoseAtTime(nowPose, mSkeleton, mAnimTime);
	// 進行具合に応じて取得
	mBlendAnimation->GetGlobalPoseAtTime(nextPose, mSkeleton, mBlendAnimTime);

	goalPose.resize(nowPose.size());

	for (size_t i = 0; i < nowPose.size(); i++)
	{
		// BoneTransformに変換して補間（Lerp/Slerp）
		BoneTransform transformA, transformB;
		transformA.FromMatrix(nowPose[i]);
		transformB.FromMatrix(nextPose[i]);

		BoneTransform blended = BoneTransform::Interpolate(transformA, transformB, t);

		goalPose[i] = blended.ToMatrix();
		mSkeleton->GetBoneActor()[i]->SetBoneMatrix(goalPose[i]);
	}

	mSkeleton->SetGlobalCurrentPoses(goalPose);

	for (size_t i = 0; i < mSkeleton->GetNumBones(); i++)
	{
		Matrix4 pose = goalPose[i];
		mPalette.mEntry[i] = globalInvBindPoses[i] * pose;
		Matrix4 transform;
		if (mOwner->GetParentActor() == nullptr)
		{
			transform = pose;
		}
		else
		{
			transform = pose * mOwner->GetWorldTransform();
		}
		mSkeleton->GetBoneActor()[i]->SetLocalScale(transform.GetScale());
		mSkeleton->GetBoneActor()[i]->SetLocalRotation(transform.GetRotation());
		mSkeleton->GetBoneActor()[i]->SetLocalPosition(transform.GetTranslation());
	}
}

float Animator::GetNormalizedTime()
{
	return mAnimTime / mAnimation->GetDuration();
}
