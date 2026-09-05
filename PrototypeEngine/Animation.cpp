#include "Animation.h"
#include "FilePath.h"
#include "Skeleton.h"
#include "BoneActor.h"

Animation::Animation(SkeletonData* skeleton)
    : mSkeleton(skeleton)
    , isRootMotion(false)
    , mRootMotionX(0.0f)
    , mRootMotionY(0.0f)
    , mRootMotionZ(0.0f)
    , isLoop(false)
    , isAnimationEnd(false)
    , isReLoad(false)
    , mDuration(0.0f)
    , mNumFrames(0)
    , mNumBones(0)
    , mFrameDuration(0.0f)
    , mFileName("")
    , mAnimationName("")
    , mRootPositionOffset()
    , isRootMotionX(false)
    , isRootMotionY(false)
    , isRootMotionZ(false)
    , mTracks()
    , mRootPosition()
{
}

bool Animation::Load(const string& fileName)
{
    mFileName = fileName;
    // ファイルの拡張子を取得
    string extension = fileName.substr(fileName.find_last_of('.') + 1);

    // **FBX の場合**
    if (extension == "fbx")
    {
        return LoadFromFBX(fileName);
    }

    return false;
}

bool Animation::ReLoad()
{
    // ファイルの拡張子を取得
    string extension = mFileName.substr(mFileName.find_last_of('.') + 1);

    // **FBX の場合**
    if (extension == "fbx")
    {
        return LoadFromFBX(mFileName);
    }

    return false;
}

bool Animation::LoadFromBinary(const std::string& filePath)
{
    filesystem::path path(filePath);
    filesystem::path animBinPath = "Binary/anim/" + filePath;
    std::ifstream in(animBinPath,std::ios::binary);
    if (!in)
    {
        SDL_Log("Failed to open animation bin: %s", animBinPath.c_str());
        return false;
    }

    AnimationBinHeader header;
    in.read((char*)&header, sizeof(header));

    if (header.version != 1)
    {
        SDL_Log("Unsupported animation version: %d", header.version);
        return false;
    }

    mDuration      = header.duration;
    mNumFrames     = header.numFrames;
    mNumBones      = mSkeleton->GetBones().size();
    if (mNumFrames > 1)
    {
        mFrameDuration = mDuration / (mNumFrames - 1);
    }
    else
    {
        mFrameDuration = 0.0f;
    }

    mTracks.resize(mNumBones);
    mRootPositionOffset.resize(mNumFrames);
    for (size_t bone = 0; bone < mNumBones; ++bone)
    {
        mTracks[bone].resize(mNumFrames);
        for (size_t frame = 0; frame < mNumFrames; ++frame)
        {
            AnimationBinTransform transform;
            in.read((char*)&transform, sizeof(transform));

            BoneTransform bt;
            bt.SetPosition(transform.position);
            bt.SetRotation(transform.rotation);
            bt.SetScale(transform.scale);
            mTracks[bone][frame] = bt;
        }
    }

    const auto& bones = mSkeleton->GetBones();
    for (size_t bone = 0; bone < mNumFrames; ++bone)
    {
        if (bones[bone].sParentIndex < 0)
        {
            Vector3 basePos = mTracks[bone][0].GetPosition();
            for (size_t frame = 0; frame < mNumFrames; ++frame)
            {
                Vector3 currentPos = mTracks[bone][frame].GetPosition();
                mRootPositionOffset[frame] = currentPos - basePos;
            }
            break;
        }
    }

    return true;
}

bool Animation::SaveToBinary(const std::string& filePath)
{
    std::ofstream out(filePath, std::ios::binary);
    if (!out)
    {
        SDL_Log("Failed to open animation bin for writing: %s",
                filePath.c_str());
        return false;
    }

    AnimationBinHeader header;
    header.duration  = mDuration;
    header.numFrames = static_cast<uint32_t>(mNumFrames);
    header.numBones  = static_cast<uint32_t>(mNumBones);
    out.write((char*)&header, sizeof(header));

    for (size_t bone = 0; bone < mNumBones; ++bone)
    {
        for (size_t frame = 0; frame < mNumFrames; ++frame)
        {
            AnimationBinTransform transform;
            transform.position = mTracks[bone][frame].GetPosition();
            transform.rotation = mTracks[bone][frame].GetRotation();
            transform.scale    = mTracks[bone][frame].GetScale();
            out.write((char*)&transform, sizeof(transform));
        }
    }

    return true;
}

void Animation::Update()
{
    if (isReLoad)
    {
        ReLoad();
        isReLoad = false;
    }
}

void Animation::Evaluate(size_t index, float time, Vector3& outpos,
                         Quaternion& outrot, Vector3& outscale)
{
    if (index >= mNumBones || mTracks[index].empty())return;

    size_t frame = 0;
    size_t nextFrame = 0;
    float  pct       = 0.0f;

    if (mFrameDuration > 0.0001f)
    {
        frame     = static_cast<size_t>(time / mFrameDuration);
        nextFrame = frame + 1;
        pct       = time / mFrameDuration - frame;
    }

    BoneTransform currentBone;
    BoneTransform nextBone;

    if (frame >= mTracks[index].size() || nextFrame >= mTracks[index].size())
    {
        currentBone = mTracks[index].back();
        nextBone    = currentBone;
    }
    else
    {
        currentBone = mTracks[index][frame];
        nextBone   = mTracks[index][nextFrame];

        //ルートボーンかつルートモーションが無効な場合、移動オフセットを引く
        const auto& bones = mSkeleton->GetBones();
        if (bones[index].sParentIndex < 0 && !isRootMotion)
        {
            currentBone.SetPosition(currentBone.GetPosition() - mRootPositionOffset[frame]);
            nextBone.SetPosition(nextBone.GetPosition() - mRootPositionOffset[nextFrame]);
        }
    }
    // 補間処理
    BoneTransform interp = BoneTransform::Interpolate(currentBone, nextBone, pct);
    outpos   = interp.GetPosition();
    outrot   = interp.GetRotation();
    outscale = interp.GetScale();
}

bool Animation::LoadFromFBX(const string& fileName)
{
    Assimp::Importer importer;
    const aiScene*   scene = importer.ReadFile(
        fileName, aiProcess_Triangulate | aiProcess_LimitBoneWeights |
                      aiProcess_GlobalScale | aiProcess_MakeLeftHanded |
                      aiProcess_FlipUVs | aiProcess_FlipWindingOrder);

    if (!scene || !scene->HasAnimations())
    {
        SDL_Log("No animations found in FBX: %s", fileName.c_str());
        return false;
    }

    aiAnimation* anim = scene->mAnimations[0];

    float ticksPerSecond = (anim->mTicksPerSecond != 0)
                               ? anim->mTicksPerSecond
                               : AnimationLayout::DEFAULTTICKSPERSECOND;
    mDuration            = static_cast<float>(anim->mDuration / ticksPerSecond);

    // mNumFrames をキーの最大値に合わせる
    mNumFrames = 0;
    for (unsigned int i = 0; i < anim->mNumChannels; i++)
    {
        aiNodeAnim* channel = anim->mChannels[i];
        mNumFrames =
            std::max((unsigned int)mNumFrames, channel->mNumPositionKeys);
        mNumFrames =
            std::max((unsigned int)mNumFrames, channel->mNumRotationKeys);
        mNumFrames =
            std::max((unsigned int)mNumFrames, channel->mNumScalingKeys);
    }

    if (mNumFrames > 1)
    {
        mFrameDuration = mDuration / (mNumFrames - 1);
    }
    else
    {
        mFrameDuration = 0.0f;
    }

    const auto& bones = mSkeleton->GetBones();
    mNumBones      = bones.size();
    mTracks.resize(mNumBones);
    mRootPositionOffset.resize(mNumFrames);

    // アニメーションに含まれていないボーンのためにバインドポーズをそのまま利用
    for (unsigned int i = 0; i < mNumBones; i++)
    {
        mTracks[i].resize(mNumFrames);

        BoneTransform localBindPose;
        localBindPose.SetPosition(bones[i].sLocalPos);
        localBindPose.SetRotation(bones[i].sLocalRot);
        localBindPose.SetScale(bones[i].sLocalScale);

        for (size_t j = 0; j < mNumFrames; j++)
        {
            mTracks[i][j] = localBindPose;
        }
    }

    // 各ボーンのアニメーションを取得
    for (unsigned int i = 0; i < anim->mNumChannels; i++)
    {
        aiNodeAnim* channel  = anim->mChannels[i];
        string      boneName = channel->mNodeName.C_Str();

        auto it = mSkeleton->GetBoneNameToIndex().find(boneName);
        if (it == mSkeleton->GetBoneNameToIndex().end())
        {
            SDL_Log("Bone %s not found in skeleton.", boneName.c_str());
            continue;
        }
        int boneIndex = it->second;

        mTracks[boneIndex].resize(mNumFrames);

        // 計算を全て補間を利用
        //  フレームごとに `BoneTransform` を作成
        for (size_t j = 0; j < mNumFrames; j++)
        {
            BoneTransform temp;
            temp.SetPosition(bones[boneIndex].sLocalPos);
            temp.SetRotation(bones[boneIndex].sLocalRot);
            temp.SetScale(bones[boneIndex].sLocalScale);

            // 位置キーの適用
            aiVector3D pos;
            CalcInterpolatedTranslation(pos, j, channel);

            // ルートモーションの無効(Y方向)
            Vector3    finalPos = Vector3();
            aiVector3D basePos  = channel->mPositionKeys[0].mValue;
            // ルートモーションの座標を値で持っておいて後で適用、不適用にする
            if (bones[boneIndex].sParentIndex < 0)
            {
                // 位置の違うモデルのために変化量を計算して利用
                // このままだと初期状態で移動している場合は適用されない！
                // 本来はボーンの元の状態から変化を計算する
                // ※現状はまだ未修整
                finalPos = Vector3(pos.x - basePos.x, pos.y - basePos.y,
                                   pos.z - basePos.z);

                mRootPositionOffset[j] = finalPos;
            }
            temp.SetPosition(temp.GetPosition() + finalPos);

            // 回転キーの適用
            aiQuaternion rot;
            CalcInterpolatedRotation(rot, j, channel);

            temp.SetRotation(Quaternion(rot.x, rot.y, rot.z, rot.w));

            // スケールキーの適用
            aiVector3D scale;
            CalcInterpolatedScaling(scale, j, channel);
            temp.SetScale(Vector3(scale.x, scale.y, scale.z));

            // `emplace_back()` ではなく、インデックス代入
            mTracks[boneIndex][j] = temp;
        }
    }

    // ファイル名から拡張子を除いてアニメーション名として登録
    mAnimationName = Sco::RemoveString(fileName, File_P::AnimationFilePath);
    mAnimationName = Sco::RemoveExtension(mAnimationName);
    return true;
}
/*
void Animation::GetLocalPoseAtTime(vector<Matrix4>& outPoses,
                                    const SkeletonData*  inSkeleton,
                                    float            inTime) const
{
    outPoses.resize(inSkeleton->GetBones().size());

    // 現在のフレームインデックスと次のフレームを特定します
    // （これは、inTimeが[0, AnimDuration]に制約されていると仮定しています）
    size_t frame     = static_cast<size_t>(inTime / mFrameDuration);
    size_t nextFrame = frame + 1;
    // フレームと次のフレームの間の分数値を計算する
    float pct = inTime / mFrameDuration - frame;
    
    // ルートのポーズを設定する
    //if (mTracks[0].size() > 0)
    //{
    //    // nextFrameが最大数を超えていることがあるため対策。
    //    if (frame >= mTracks[0].size() || nextFrame >= mTracks[0].size())
    //    {
    //        outPoses[0] = mTracks[0][mTracks[0].size() - 1].ToMatrix();
    //    }
    //    else
    //    {
    //        // 現在のフレームのポーズと次のフレームの間を補間する。
    //        BoneTransform interp = BoneTransform::Interpolate(
    //            mTracks[0][frame], mTracks[0][nextFrame], pct);
    //        outPoses[0] = interp.ToMatrix();
    //    }
    //}
    //else
    //{
    //    outPoses[0] = Matrix4::Identity;
    //}
    

    const vector<BoneActor*> bones = inSkeleton->GetBoneActor();
    // 残りのポーズを設定。
    for (size_t bone = 0; bone < mNumBones; bone++)
    {
        if (mTracks[bone].size() > 0)
        {
            BoneTransform currentBone;
            BoneTransform nextBone;


            // nextFrameが最大数を超えていることがあるため対策
            if (frame >= mTracks[bone].size() ||
                nextFrame >= mTracks[bone].size())
            {
                currentBone = mTracks[bone].back();
                nextBone    = currentBone;
                
                //if (bones[bone]->GetParentIndex() < 0)
                //{
                //    if (!isRootMotion)
                //    {
                //        currentBone.SetPosition(
                //            currentBone.GetPosition() -=
                //            mRootPositionOffset[mTracks[bone].size() - 1]);
                //    }
                //}
                //localMat = currentBone.ToMatrix();
                
            }
            else
            {
                currentBone = mTracks[bone][frame];
                nextBone    = mTracks[bone][nextFrame];
                
                //if (bones[bone]->GetParentIndex() < 0)
                //{
                //    if (!isRootMotion)
                //    {
                //        // currentBone.mPosition -= mRootPositionOffset[frame];
                //        currentBone.SetPosition(currentBone.GetPosition() -=
                //                                mRootPositionOffset[frame]);
                //        // nextBone.mPosition -= mRootPositionOffset[nextFrame];
                //        nextBone.SetPosition(nextBone.GetPosition() -=
                //                             mRootPositionOffset[nextFrame]);
                //    }
                //}
                //BoneTransform interp =
                //    BoneTransform::Interpolate(currentBone, nextBone, pct);
                //localMat = interp.ToMatrix();
                
            }

            if (bones[bone]->GetParentIndex() < 0 && !isRootMotion)
            {
                currentBone.SetPosition(currentBone.GetPosition() - mRootPositionOffset[frame]);
                nextBone.SetPosition(nextBone.GetPosition() - mRootPositionOffset[nextFrame]);
            }

            BoneTransform interp =
                BoneTransform::Interpolate(currentBone, nextBone, pct);

            outPoses[bone] = interp.ToMatrix();
        }
        else
        {
            outPoses[bone] = Matrix4::Identity;
        }
        
        // 親がいない場合の対処
        //if (bones[bone]->GetParentIndex() < 0)
        //{
        //    outPoses[bone] = localMat;
        //    continue;
        //}
        //
        //outPoses[bone] = localMat * outPoses[bones[bone]->GetParentIndex()];
        
    }
}
*/

// 補間情報の計算、チュートリアルから引用
//  https://ogldev.org/www/tutorial38/tutorial38.html
size_t Animation::FindTranslation(float             AnimationTime,
                                  const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (size_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
    {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
        {
            return i;
        }
    }

    assert(0);
    return 0;
}
void Animation::CalcInterpolatedTranslation(aiVector3D&       Out,
                                            float             AnimationTime,
                                            const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1)
    {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }
    if (AnimationTime >=
        pNodeAnim->mPositionKeys[pNodeAnim->mNumPositionKeys - 1].mTime)
    {
        Out = pNodeAnim->mPositionKeys[pNodeAnim->mNumPositionKeys - 1].mValue;
        return;
    }

    size_t TranslationIndex = FindTranslation(AnimationTime, pNodeAnim);

    size_t NextTranslationIndex = (TranslationIndex + 1);

    assert(NextTranslationIndex < pNodeAnim->mNumPositionKeys);

    float DeltaTime = pNodeAnim->mPositionKeys[NextTranslationIndex].mTime -
                      pNodeAnim->mPositionKeys[TranslationIndex].mTime;

    float Factor = (AnimationTime -
                    (float)pNodeAnim->mPositionKeys[TranslationIndex].mTime) /
                   DeltaTime;

    assert(Factor >= 0.0f && Factor <= 1.0f);

    const aiVector3D& StartTranslation =
        pNodeAnim->mPositionKeys[TranslationIndex].mValue;

    const aiVector3D& EndTranslation =
        pNodeAnim->mPositionKeys[NextTranslationIndex].mValue;

    Out = StartTranslation + (EndTranslation - StartTranslation) * Factor;
}

void Animation::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime,
                                         const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumRotationKeys == 1)
    {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }
    if (AnimationTime >=
        pNodeAnim->mRotationKeys[pNodeAnim->mNumRotationKeys - 1].mTime)
    {
        Out = pNodeAnim->mRotationKeys[pNodeAnim->mNumRotationKeys - 1].mValue;
        return;
    }

    size_t RotationIndex = FindRotation(AnimationTime, pNodeAnim);

    size_t NextRotationIndex = (RotationIndex + 1);

    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);

    float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime -
                      pNodeAnim->mRotationKeys[RotationIndex].mTime;

    float Factor =
        (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) /
        DeltaTime;

    assert(Factor >= 0.0f && Factor <= 1.0f);

    const aiQuaternion& StartRotationQ =
        pNodeAnim->mRotationKeys[RotationIndex].mValue;

    const aiQuaternion& EndRotationQ =
        pNodeAnim->mRotationKeys[NextRotationIndex].mValue;

    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);

    Out = Out.Normalize();
}
size_t Animation::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (size_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
    {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
        {
            return i;
        }
    }

    assert(0);
    return 0;
}

void Animation::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime,
                                        const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1)
    {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }
    if (AnimationTime >=
        pNodeAnim->mScalingKeys[pNodeAnim->mNumScalingKeys - 1].mTime)
    {
        Out = pNodeAnim->mScalingKeys[pNodeAnim->mNumScalingKeys - 1].mValue;
        return;
    }

    size_t ScalingIndex = FindScaling(AnimationTime, pNodeAnim);

    size_t NextScalingIndex = (ScalingIndex + 1);

    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);

    float DeltaTime = pNodeAnim->mScalingKeys[NextScalingIndex].mTime -
                      pNodeAnim->mScalingKeys[ScalingIndex].mTime;

    float Factor =
        (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) /
        DeltaTime;

    assert(Factor >= 0.0f && Factor <= 1.0f);

    const aiVector3D& StartScaling =
        pNodeAnim->mScalingKeys[ScalingIndex].mValue;

    const aiVector3D& EndScaling =
        pNodeAnim->mScalingKeys[NextScalingIndex].mValue;

    Out = StartScaling + (EndScaling - StartScaling) * Factor;
}
size_t Animation::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumPositionKeys > 0);

    for (size_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
    {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
        {
            return i;
        }
    }
    assert(0);
    return 0;
}