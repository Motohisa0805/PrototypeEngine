#include "Animator.h"
#include "BoneActor.h"
#include "DebugManager.h"
#include "FilePath.h"
#include "Skeleton.h"

Animator::Animator(Entity* owner)
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
    mName = "Animator";
}

Animator::~Animator()
{
    for (int i = 0; i < mAnimations.size(); i++)
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
bool Animator::Load(const string& fileName, bool animLoop, bool rootMotion)
{
    string     path = File_P::AnimationFilePath + fileName;
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
    /*
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
            mAnimTime       = mBlendAnimTime;
            mAnimation      = mBlendAnimation;
            mBlendAnimation = nullptr;
            mBlending       = false;
        }
    }
    */

    if (!mAnimation || mBoneTransforms.empty()) return;
    //経過時間の更新
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
    //対象ボーンのTransformを更新
    for (size_t i = 0; i < mBoneTransforms.size(); i++)
    {
        Transform* boneTransform = mBoneTransforms[i];
        //アニメーションから現在のローカル値を取得
        Vector3 pos; Quaternion rot; Vector3 scale;
        mAnimation->Evaluate(i, mAnimTime, pos, rot, scale);

        boneTransform->SetLocalPosition(pos);
        boneTransform->SetLocalRotation(rot);
        boneTransform->SetLocalScale(scale);
    }
}

void Animator::SetSkeleton(SkeletonData* skeleton)
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
    if (mAnimation == anim)
    {
        return 0.0f;
    }
    mAnimation = anim;
    mAnimTime  = 0.0f;

    mAnimation->SetIsAnimationEnd(false);

    if (!mAnimation)
    {
        return 0.0f;
    }

    //ComputeMatrixPalette();

    return mAnimation->GetDuration();
}

float Animator::PlayBlendAnimation(Animation* anim)
{
    if (mAnimation == anim || mBlendAnimation == anim)
    {
        return 0.0f;
    }

    mBlendAnimation = anim;
    mBlendAnimTime  = 0.0f;

    mBlendAnimation->SetIsAnimationEnd(false);
    mAnimation->SetIsAnimationEnd(false);
    mBlending = true;

    return mAnimation->GetDuration();
}
/*
void Animator::ComputeMatrixPalette()
{
    if (!mSkeleton || !mAnimation)
    {
        return;
    }

    vector<Matrix4> localPoses;
    mAnimation->GetLocalPoseAtTime(localPoses, mSkeleton, mAnimTime);
    
    //const vector<Matrix4>& globalInvBindPoses =
    //    mSkeleton->GetGlobalInvBindPoses();
    //vector<Matrix4> currentPoses;
    //mAnimation->GetLocalPoseAtTime(currentPoses, mSkeleton, mAnimTime);
    //mSkeleton->SetGlobalCurrentPoses(currentPoses);
    
    // Setup the palette for each bone
    for (size_t i = 0; i < mSkeleton->GetBones().size(); i++)
    {
        BoneActor* boneActor = mSkeleton->GetBoneActor()[i];
        
        Vector3 pos = localPoses[i].GetTranslation();
        Quaternion rot = localPoses[i].GetRotation();
        Vector3    scale = localPoses[i].GetScale();

        boneActor->GetTransform()->SetLocalPosition(pos);
        boneActor->GetTransform()->SetLocalRotation(rot);
        boneActor->GetTransform()->SetLocalScale(scale);

        boneActor->GetTransform()->ActiveDirty();
        
        
        //Matrix4 pose = currentPoses[i];
        //// Global inverse bind pose matrix times current pose matrix
        //mPalette.mEntry[i] = globalInvBindPoses[i] * pose;
        //Matrix4 transform;
        //if (mActor->GetTransform()->GetParentActor() == nullptr)
        //{
        //    transform = pose;
        //}
        //else
        //{
        //    transform = pose * mActor->GetTransform()->GetWorldTransform();
        //}
        //mSkeleton->GetBoneActor()[i]->GetTransform()->SetLocalScale(
        //    transform.GetScale());
        //mSkeleton->GetBoneActor()[i]->GetTransform()->SetLocalRotation(
        //    transform.GetRotation());
        //mSkeleton->GetBoneActor()[i]->GetTransform()->SetLocalPosition(
        //    transform.GetTranslation());
        
    }

    for (size_t i = 0; i < mSkeleton->GetNumBones(); i++)
    {
        mSkeleton->GetBoneActor()[i]->GetTransform()->ComputeWorldTransform();
    }

    for (size_t i = 0; i < mSkeleton->GetNumBones(); i++)
    {
        BoneActor* boneActor = mSkeleton->GetBoneActor()[i];
        Matrix4 currentWorld = boneActor->GetTransform()->GetWorldTransform();
        Matrix4 invBind      = boneActor->GetGlobalInvBindPose();

        mPalette.mEntry[i] = invBind * currentWorld;
    }
}

void Animator::BlendComputeMatrixPalette()
{
    if (!mSkeleton || !mAnimation || !mBlendAnimation)
    {
        return;
    }

    vector<Matrix4> nowPose;
    vector<Matrix4> nextPose;
        // アニメーションタイムを使ってそれぞれのポーズを取得
    mAnimation->GetLocalPoseAtTime(nowPose, mSkeleton, mAnimTime);
    // 進行具合に応じて取得
    mBlendAnimation->GetLocalPoseAtTime(nextPose, mSkeleton, mBlendAnimTime);
    // 経過時間に対する補間率
    float t = Math::Clamp(mBlendAnimTime / mBlendElapsed, 0.0f, 1.0f);

    for (size_t i = 0; i < mSkeleton->GetNumBones(); i++)
    {
        BoneTransform transformA, transformB;
        transformA.FromMatrix(nowPose[i]);
        transformB.FromMatrix(nextPose[i]);

        BoneTransform blended =
            BoneTransform::Interpolate(transformA, transformB, t);

        BoneActor* bone = mSkeleton->GetBoneActor()[i];
        bone->GetTransform()->SetLocalPosition(blended.GetPosition());
        bone->GetTransform()->SetLocalRotation(blended.GetRotation());
        bone->GetTransform()->SetLocalScale(blended.GetScale());
        bone->GetTransform()->ActiveDirty();
    }

    for (size_t i = 0; i < mSkeleton->GetNumBones(); i++)
    {
        mSkeleton->GetBoneActor()[i]->GetTransform()->ComputeWorldTransform();
    }

    for (size_t i = 0; i < mSkeleton->GetNumBones(); i++)
    {
        BoneActor* bone = mSkeleton->GetBoneActor()[i];
        mPalette.mEntry[i] = bone->GetGlobalInvBindPose() *
                             bone->GetTransform()->GetWorldTransform();
    }
    
    //vector<Matrix4> goalPose;
    //
    //
    //goalPose.resize(nowPose.size());
    //
    //for (size_t i = 0; i < nowPose.size(); i++)
    //{
    //    // BoneTransformに変換して補間（Lerp/Slerp）
    //    BoneTransform transformA, transformB;
    //    transformA.FromMatrix(nowPose[i]);
    //    transformB.FromMatrix(nextPose[i]);
    //
    //    BoneTransform blended =
    //        BoneTransform::Interpolate(transformA, transformB, t);
    //
    //    goalPose[i] = blended.ToMatrix();
    //    mSkeleton->GetBoneActor()[i]->SetGlobalInvBindPose(goalPose[i]);
    //}
    //
    //mSkeleton->SetGlobalCurrentPoses(goalPose);
    //
    //for (size_t i = 0; i < mSkeleton->GetNumBones(); i++)
    //{
    //    Matrix4 pose       = goalPose[i];
    //    mPalette.mEntry[i] = globalInvBindPoses[i] * pose;
    //    Matrix4 transform;
    //    if (mActor->GetTransform()->GetParentActor() == nullptr)
    //    {
    //        transform = pose;
    //    }
    //    else
    //    {
    //        transform = pose * mActor->GetTransform()->GetWorldTransform();
    //    }
    //    mSkeleton->GetBoneActor()[i]->GetTransform()->SetLocalScale(
    //        transform.GetScale());
    //    mSkeleton->GetBoneActor()[i]->GetTransform()->SetLocalRotation(
    //        transform.GetRotation());
    //    mSkeleton->GetBoneActor()[i]->GetTransform()->SetLocalPosition(
    //        transform.GetTranslation());
    //}
}
*/

float Animator::GetNormalizedTime()
{
    return mAnimTime / mAnimation->GetDuration();
}

void Animator::Serialize(json& j) const 
{
    Component::Serialize(j);
}

void Animator::Deserialize(const json& j) 
{
    Component::Deserialize(j);
}

void Animator::DrawCustomGUI(const std::vector<PropertyInfo>& properties) 
{
    ImGui::PushID(this);

    ImGui::Text("No Properties");

    ImGui::PopID();
}

Component* Animator::Clone(Entity* newOwner) const 
{
    Animator* clone = new Animator(newOwner);

    clone->mAnimations.resize(this->mAnimations.size(), nullptr);
    for (int i = 0; i < this->mAnimations.size(); ++i)
    {
        clone->mAnimations[i] = this->mAnimations[i];
    }

    clone->mSkeleton   = this->mSkeleton;
    clone->mAnimation  = this->mAnimation;
    clone->mBlendAnimation = this->mBlendAnimation;
    clone->mAnimPlayRate   = this->mAnimPlayRate;
    clone->mAnimTime       = this->mAnimTime;
    clone->mBlendAnimTime  = this->mBlendAnimTime;
    clone->mBlendElapsed   = this->mBlendElapsed;
    clone->mBlending       = this->mBlending;

    return clone;
}
