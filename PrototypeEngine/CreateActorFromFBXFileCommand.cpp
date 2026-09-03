#include "CreateActorFromFBXFileCommand.h"
#include "MeshRenderer.h"
#include "SkeletalMeshRenderer.h"
#include "AssetDataBase.h"
#include "SceneManager.h"
#include <filesystem>
#include <nlohmann/json.hpp>
#include "DebugManager.h"
#include "CreateActorTemplate.h"

CreateActorFromFBXFileCommand::CreateActorFromFBXFileCommand(
    const std::filesystem::path& assetPath, ActorObject* parentActor)
    : mAssetPath(assetPath)
    , mParentActor(parentActor)
    , mIsActiveInScene(false)
    , mTargetIDs()
    , mTargets()
{
}

CreateActorFromFBXFileCommand::~CreateActorFromFBXFileCommand() 
{
    if (!mIsActiveInScene)
    {
        for (ActorObject* target : mTargets)
        {
            delete target;
        }
        mTargets.clear();
    }
}

void CreateActorFromFBXFileCommand::Execute() 
{
    ActorManager* actorManager = SceneManager::GetCurrentRunScene()->GetActorManager();
    //初回時
    if (mTargetIDs.empty())
    {
        //.metaファイルから階層を読み込み
        std::filesystem::path metaPath = AssetDataBase::GetInstance().GeneratedMetaFilePath(mAssetPath);
        if (!std::filesystem::exists(metaPath))
        {
            Debug::ErrorLog("Meta file does not exist: %s", metaPath.string().c_str());
            return;
        }

        std::ifstream inFile(metaPath);
        nlohmann::json metaJson;
        if (inFile.is_open())
        {
            inFile >> metaJson;
            inFile.close();
        }
        else
        {
            Debug::ErrorLog("Failed to open meta file: %s", metaPath.string().c_str());
            return;
        }

        const auto& hierarchyJson = metaJson["cached_data"]["hierarchy"];

        bool isSkeletonImport = metaJson["import_settings"]["import_skeleton"];
        if (isSkeletonImport)
        {
            CreateActorTemplate::CreateSkeletonActor(metaJson,hierarchyJson, mParentActor, mAssetPath, mTargetIDs);
        }
        else
        {
            //再帰的にアクターを生成し、親子関係を構築する関数
            CreateActorTemplate::CreateFBXFileActor(hierarchyJson,mParentActor,mAssetPath,mTargetIDs);
        }

        mIsActiveInScene = true;
    }
    //Redo時
    else
    {
        if (!mIsActiveInScene && !mTargets.empty())
        {
            for (ActorObject* actor : mTargets)
            {
                actorManager->ReAddActor(actor);
            }

            mTargets.clear();
            mIsActiveInScene = true;
        }
    }
}

void CreateActorFromFBXFileCommand::Undo() 
{
    if(mTargetIDs.empty() || !mIsActiveInScene)return;

    ActorManager* actorManager = SceneManager::GetCurrentRunScene()->GetActorManager();
    //IDを元に消す
    for (uint64_t id : mTargetIDs)
    {
        ActorObject* currentActor = actorManager->FindActorByID(id);
        if (currentActor)
        {
            actorManager->DetachActor(currentActor);

            mTargets.push_back(currentActor);
        }
    }

    mIsActiveInScene = false;

    //もし生成したアクターが現在選択されていたら解除する
    Entity* selected = SelectionManager::GetSelectedActor();
    for (ActorObject* target : mTargets)
    {
        if (selected == target)
        {
            SelectionManager::SetSelectedActor(nullptr);
            break;
        }
    }
}

void CreateActorFromFBXFileCommand::Redo() { Execute(); }
