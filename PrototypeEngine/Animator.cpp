#include "Animator.h"
#include "BoneActor.h"
#include "DebugManager.h"
#include "FilePath.h"
#include "Skeleton.h"
#include "AssetDataBase.h"
#include "AnimatorNodeEditorPanel.h"

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
    , mControllerData()
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
    if (AnimatorNodeEditorPanel::GetSelectAnimator() == this)
    {
        AnimatorNodeEditorPanel::SetSelectAnimator(nullptr);
    }
}

bool Animator::LoadController(const string& filePath) 
{
    AssetMetaData meta = AssetDataBase::GetInstance().GetAssetMetaData(filePath);
    mControllerGUID = meta.sGUID;

    std::ifstream file(filePath);
    if (!file.is_open())
    {
        Debug::Log("Failed to open controller file: %s", filePath.c_str());
        return false;
    }

    try
    {
        nlohmann::json j;
        file >> j;
        mControllerData = j.get<AnimatorControllerParameters>();

        //パラメーターマップの初期化
        mCurrentParameters.clear();
        for (const auto& param : mControllerData.sParameters)
        {
            mCurrentParameters[param.sName] = param;
        }

        for (auto* anim : mAnimations)
        {
            delete anim;
        }
        mAnimations.clear();
        mAnimationInfo.clear();
        mStateAnimations.clear();

        for (const auto& state : mControllerData.sStates)
        {
            if (state.sAnimInfo.sBinayPath.empty())continue;

            Animation* anim = new Animation(mSkeleton);
            anim->SetLoop(state.sAnimInfo.sIsLoop);
            anim->SetRootMotion(state.sAnimInfo.sRootMotion);

            if (anim->LoadFromBinary(state.sAnimInfo.sBinayPath.string()))
            {
                mAnimations.push_back(anim);
                mStateAnimations[state.sStateName] = anim;

                AnimInfo info{};
                info.sBinayPath = anim->GetFilePath();
                info.sIsLoop = anim->IsLoop();
                info.sRootMotion = anim->IsRootMotion();
                mAnimationInfo.push_back(info);

                if (state.sStateName == mControllerData.sDefaultState)
                {
                    mAnimation = anim;
                    mAnimPlayRate = state.sPlaybackSpeed;
                    mAnimTime     = 0.0f;
                }
            }
            else
            {
                delete anim;
            }
        }

        for (const auto& trans : mControllerData.sTransitions)
        {
            if (trans.sFromState == "Entry")
            {
                mCurrentStateName = trans.sToState;
                break;
            }
        }
        //見つからなければdefaultStateを使用
        if (mCurrentStateName.empty())
        {
            mCurrentStateName = mControllerData.sDefaultState;
        }

        if (mStateAnimations.count(mCurrentStateName))
        {
            mAnimation = mStateAnimations[mCurrentStateName];
            mAnimPlayRate = 1.0f;
            mAnimTime     = 0.0f;
        }
        
        return true; 
    }
    catch (const std::exception& e)
    {
        Debug::Log("JSON Parse Error in Controller: %s", e.what());
        return false; 
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
    string     path = fileName;
    Animation* anim = new Animation(mSkeleton);
    anim->SetLoop(animLoop);
    anim->SetRootMotion(rootMotion);

    if (anim->LoadFromBinary(fileName))
    {
        AddAnimation(anim);
        return true;
    }
    else
    {
        delete anim;
        anim = nullptr;
    }
    return false;
}

void Animator::CheckStateTransitions() 
{
    //ブレンド中なら二重に遷移しない
    if (mBlending) return;
    //次の遷移先を探す
    for (const auto& trans : mControllerData.sTransitions)
    {
        if (trans.sFromState == mCurrentStateName || trans.sFromState == "Any State")
        {
            // 条件が無い場合：アニメーションの再生を待つ
            if (trans.sConditions.empty())
            {
                if (!mAnimation->IsAnimationEnd() &&
                    mAnimTime < mAnimation->GetDuration())
                {
                    continue;
                }
            }
            else
            {
                bool allConditionsMat = true;
                for (const auto& cond : trans.sConditions)
                {
                    if (!EvaluateCondition(cond))
                    {
                        allConditionsMat = false;
                        break;
                    }
                }
                if (!allConditionsMat) continue;
            }
            //遷移決定
            for (const auto& cond : trans.sConditions)
            {
                if (mCurrentParameters.count(cond.sParameterName))
                {
                    if (mCurrentParameters[cond.sParameterName].sType == AnimParamType::sTrigger)
                    {
                        mCurrentParameters[cond.sParameterName].sDefaultTrigger = false;
                    }
                }
            }

            //次の遷移先決定
            string nextState = trans.sToState;
            if (nextState == "Exit")
            {
                for (const auto& entryTrans : mControllerData.sTransitions)
                {
                    if (entryTrans.sFromState == "Entry")
                    {
                        nextState = entryTrans.sToState;
                        break;
                    }
                }
            }

            if (mStateAnimations.count(nextState) && nextState != mCurrentStateName)
            {
                mCurrentStateName = nextState;

                for (const auto& state : mControllerData.sStates)
                {
                    if (state.sStateName == nextState)
                    {
                        mAnimPlayRate = state.sPlaybackSpeed;
                        break;
                    }
                }
                mBlendElapsed = trans.sBlendDuration;
                PlayBlendAnimation(mStateAnimations[nextState]);
                break;
            }
        }
    }
}

bool Animator::EvaluateCondition(const AnimCondition& cond) 
{
    if (!mCurrentParameters.count(cond.sParameterName))return false;

    const auto& param = mCurrentParameters[cond.sParameterName];

    if (param.sType == AnimParamType::sBool || param.sType == AnimParamType::sTrigger)
    {
        if (cond.sMode == "If" || cond.sMode == "true")return param.sDefaultBool == true;
        if (cond.sMode == "IfNot" || cond.sMode == "false")return param.sDefaultBool == false;
    }
    else
    {
        float val = param.sDefaultFloat;
        float threshold = cond.sThreshold;

        if (cond.sMode == ">" || cond.sMode == "Greater")return val > threshold;
        if (cond.sMode == "<" || cond.sMode == "Less")return val < threshold;
        if (cond.sMode == "==" || cond.sMode == "Equals")return Math::Abs(val - threshold) < 0.0001f;
        if (cond.sMode == "!=" || cond.sMode == "NotEquals")return Math::Abs(val - threshold) >= 0.0001f;
    }

    return false; 
}

void Animator::Update(float deltaTime)
{
    if (!mAnimation || !mSkeleton) return;
    //もしmBonesの[0]要素がnullptrだったら
    if (mBones[0] == nullptr || mBones.empty())
    {
        ReloadBones(mActor);
    }
    //メインアニメーションの再生時間更新
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

    //毎フレーム遷移判定
    CheckStateTransitions();

    //ブレンド用サブアニメーションの再生時間更新と補間率の計算
    float       blendAlpha    = 0.0f;
    if (mBlending && mBlendAnimation)
    {
        mBlendAnimTime += deltaTime * mAnimPlayRate;
        blendAlpha = Math::Clamp(mBlendAnimTime / mBlendElapsed, 0.0f, 1.0f);

        if (mBlendAnimation->IsLoop())
        {
            if (mBlendAnimTime > mBlendAnimation->GetDuration())
                mBlendAnimTime = 0.0f;
        }
        else
        {
            if (mBlendAnimTime > mBlendAnimation->GetDuration())
            {
                mBlendAnimTime = mBlendAnimation->GetDuration();
            }
        }
    }


    const auto& skeletonBones = mSkeleton->GetBones();
    //対象ボーンのTransformを更新
    for (size_t i = 0; i < mBones.size(); i++)
    {
        if (!mBones[i]) continue;
        Transform* boneTransform = mBones[i]->GetTransform();
        //アニメーションから現在のローカル値を取得
        Vector3 posA; Quaternion rotA; Vector3 scaleA;
        mAnimation->Evaluate(i, mAnimTime, posA, rotA, scaleA);

        Vector3 finalPos = posA;
        Quaternion finalRot = rotA;
        Vector3    finalScale = scaleA;

        if (mBlending && mBlendAnimation)
        {
            Vector3 posB;Quaternion rotB;Vector3 scaleB;
            mBlendAnimation->Evaluate(i, mBlendAnimTime, posB, rotB, scaleB);

            finalPos = Vector3::Lerp(posA, posB, blendAlpha);
            finalRot = Quaternion::Slerp(rotA, rotB, blendAlpha);
            finalScale = Vector3::Lerp(scaleA, scaleB, blendAlpha);
        }



        //ルートボーンのみアニメーション位置(補間後)を適用し、子ボーンはスケルトン位置を維持
        if (skeletonBones[i].sParentIndex == -1)
        {
            boneTransform->SetLocalPosition(finalPos);
        }
        else
        {
            boneTransform->SetLocalPosition(skeletonBones[i].sLocalPos);
        }

        boneTransform->SetLocalScale(finalScale);
        boneTransform->SetLocalRotation(finalRot);
        boneTransform->ActiveDirty();
    }

    //ブレンド完了処理
    if (mBlending && mBlendAnimTime >= mBlendElapsed)
    {
        mAnimation      = mBlendAnimation;
        mAnimTime       = mBlendAnimTime;
        mBlendAnimation = nullptr;
        mBlending       = false;
    }
}

void Animator::AddAnimation(Animation* anim) 
{ 
    mAnimations.push_back(anim); 
    // 1つめのアニメーションだけは自動的に再生する
    if (mAnimations.size() == 1)
    {
        mAnimation = anim;
        mAnimation->SetLoop(true);
    }
    AnimInfo info{};
    info.sBinayPath = anim->GetFilePath();
    info.sIsLoop = anim->IsLoop();
    info.sRootMotion = anim->IsRootMotion();
    mAnimationInfo.push_back(info);
}

filesystem::path Animator::GetControllerFilePath()
{
    return AssetDataBase::GetInstance().GetAssetPathByGUID(mControllerGUID);
}

void Animator::SetFloat(const string& name, float value) 
{
    if (mCurrentParameters.count(name))mCurrentParameters[name].sDefaultFloat = value;
}

void Animator::SetBool(const string& name, bool value) 
{
    if (mCurrentParameters.count(name))mCurrentParameters[name].sDefaultBool = value;
}

void Animator::SetInt(const string& name, int value) 
{
    if (mCurrentParameters.count(name))mCurrentParameters[name].sDefaultInt = value;
}

void Animator::SetTriger(const string& name) 
{
    if (mCurrentParameters.count(name))mCurrentParameters[name].sDefaultTrigger = true;
}

float Animator::GetFloat(const string& name) 
{
    if (mCurrentParameters.count(name))
    {
        return mCurrentParameters[name].sDefaultFloat;
    }
    return 0.0f; 
}

bool Animator::GetBool(const string& name) 
{
    if (mCurrentParameters.count(name))
    {
        return mCurrentParameters[name].sDefaultBool;
    }
    return false; 
}

bool Animator::GetInt(const string& name) 
{
    if (mCurrentParameters.count(name))
    {
        return mCurrentParameters[name].sDefaultInt;
    }
    return false; 
}

void Animator::ReloadBones(ActorObject* rootbone)
{
    const auto& bones = mSkeleton->GetBones();
    mBones.resize(bones.size(), nullptr);

    for (size_t i = 0; i < bones.size(); ++i)
    {
        mBones[i] = SkeletonData::FindActorByName(rootbone, bones[i].sName);
    }
}

void Animator::LoadSkeletonData(const string& fileName, ActorObject* rootBone)
{
    SkeletonData* sk = mGame->GetSkeleton(fileName);
    mSkeleton = sk;

    mBones.clear();

    if (!mSkeleton)
    {
        return;
    }
    mSkeletonFilePath = fileName;
    ReloadBones(rootBone);
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
    mBlending = true;
    mBlendAnimation->SetIsAnimationEnd(false);

    mAnimTime = 0.0f;
    mAnimation->SetIsAnimationEnd(false);

    return mAnimation->GetDuration();
}

float Animator::GetNormalizedTime()
{
    if (!mAnimation)
    {
        return 1.0f;
    }
    return mAnimTime / mAnimation->GetDuration();
}

void Animator::Serialize(json& j) const 
{
    Component::Serialize(j);
    if (mSkeleton)
    {
        j["SkeletonPath"] = mSkeleton->GetSkeletonFilePath();
    }
    j["ControllerGUID"] = mControllerGUID;
}

void Animator::Deserialize(const json& j) 
{
    Component::Deserialize(j);
    if (j.contains("SkeletonPath"))
    {
        filesystem::path skeletonPath = j.value("SkeletonPath", "");
        mSkeletonFilePath             = skeletonPath;
    }

    if (j.contains("ControllerGUID"))
    {
        mControllerGUID = j.value("ControllerGUID", "");
    }
}

void Animator::DeserializeAfterParentChildBuild() 
{
    LoadSkeletonData(mSkeletonFilePath.string(), static_cast<ActorObject*>(mOwner));

    if (!mControllerGUID.empty())
    {
        string resolvedPath = AssetDataBase::GetInstance().GetAssetPathByGUID(mControllerGUID);
        if (!resolvedPath.empty())
        {
            LoadController(resolvedPath);
        }
        else
        {
            Debug::Log("Warning: AnimatorController asset not found for GUID: %s",mControllerGUID.c_str());
        }
    }
}

void Animator::DrawCustomGUI(const std::vector<PropertyInfo>& properties) 
{
    ImGui::PushID(this);

    // 1.ファイルパスの取得
    filesystem::path currentPath = mSkeleton != nullptr ? mSkeleton->GetSkeletonFilePath() : "";
    static char      pathBuffer[256];
    strncpy_s(pathBuffer, currentPath.filename().stem().string().c_str(),
              sizeof(pathBuffer));
    pathBuffer[sizeof(pathBuffer) - 1] = '\0';
    ImGui::Text("Avater");
    // 2.ファイルパスの入力フィールド
    ImGui::InputText("Avater File Path", pathBuffer, sizeof(pathBuffer),
                     ImGuiInputTextFlags_ReadOnly);

    // 3.ファイルロードボタン(ここでファイル選択UIを開くか、ProjectPanelからのDrag&Dropを想定)
    // Drag&Drop想定
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload("AVATAR_ITEM"))
        {
            // ペイロードがファイルパスであると仮定
            AvatarPayload data = (AvatarPayload)payload->Data;
            // ファイルパスを使いロード処理を呼び出す
            LoadSkeletonData(data.sAvatarBinaryPath.string(),
                             static_cast<ActorObject*>(mOwner));
        }
        ImGui::EndDragDropTarget();
    }
    if (ImGui::Button("Clear Avater"))
    {
        mSkeleton = nullptr;
    }

    ImGui::NewLine();
    ImGui::Separator();

    ImGui::Text("Animator Controller");
    static char controllerPathBuffer[256];
    strncpy_s(controllerPathBuffer, GetControllerFilePath().filename().string().c_str(),sizeof(controllerPathBuffer));
    controllerPathBuffer[sizeof(controllerPathBuffer) - 1] = '\0';

    ImGui::InputText("Controller", controllerPathBuffer, sizeof(controllerPathBuffer),ImGuiInputTextFlags_ReadOnly);

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
        {
            const char*      payloadPath = (const char*)payload->Data;
            filesystem::path droppedPath(payloadPath);
            LoadController(droppedPath.string());
        }

        ImGui::EndDragDropTarget();
    }

    if (ImGui::Button("Clear Controller"))
    {
        GetControllerFilePath().clear();
        mControllerGUID.clear();
        mControllerData = AnimatorControllerParameters();
        for (auto* anim : mAnimations)delete anim;
        mAnimations.clear();
        mStateAnimations.clear();
        mAnimation = nullptr;
    }

    ImGui::PopID();
}

Component* Animator::Clone(Entity* newOwner) const
{
    Animator* clone = new Animator(newOwner);

    clone->mSkeletonFilePath = this->mSkeletonFilePath;
    clone->LoadSkeletonData(mSkeletonFilePath.string(),
                            static_cast<ActorObject*>(mOwner));

    clone->mControllerGUID     = this->mControllerGUID;
    if (!clone->GetControllerFilePath().empty())
    {
        clone->LoadController(clone->GetControllerFilePath().string());
    }

    clone->mAnimPlayRate  = this->mAnimPlayRate;
    clone->mAnimTime      = this->mAnimTime;
    clone->mBlendAnimTime = this->mBlendAnimTime;
    clone->mBlendElapsed  = this->mBlendElapsed;
    clone->mBlending      = this->mBlending;

    return clone;
}
