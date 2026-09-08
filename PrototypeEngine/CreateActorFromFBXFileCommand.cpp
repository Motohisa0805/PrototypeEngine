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
    : mCreateParentID(-1)
    , mCreateParentActor(nullptr)
    , mAssetPath(assetPath)
    , mParentActor(parentActor)
    , mIsActiveInScene(false)
{
}

CreateActorFromFBXFileCommand::~CreateActorFromFBXFileCommand() 
{
    if (mCreateParentActor && !mIsActiveInScene)
    {
        ReleasePasteActor(mCreateParentActor);
    }
}

void CreateActorFromFBXFileCommand::ReleasePasteActor(ActorObject* actor) 
{
    for (ActorObject* child : actor->GetTransform()->GetChildActorList())
    {
        ReleasePasteActor(child);
    }
    delete actor;
}

void CreateActorFromFBXFileCommand::Execute() 
{
    ActorManager* actorManager = SceneManager::GetCurrentRunScene()->GetActorManager();
    //初回時
    if (mCreateParentID == -1)
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
            mCreateParentID = CreateActorTemplate::CreateSkeletonActor(metaJson, hierarchyJson, mParentActor, mAssetPath);
        }
        else
        {
            //再帰的にアクターを生成し、親子関係を構築する関数
            mCreateParentID = CreateActorTemplate::CreateFBXFileActor(hierarchyJson,mParentActor,mAssetPath);
        }

        mIsActiveInScene = true;
    }
    //Redo時
    else
    {
        if (mCreateParentActor)
        {
            actorManager->ReAddActor(mCreateParentActor);

            mCreateParentActor = nullptr;
            mIsActiveInScene = true;
        }
    }
}

void CreateActorFromFBXFileCommand::Undo() 
{
    if(mCreateParentID == -1 || !mIsActiveInScene)return;

    ActorManager* actorManager = SceneManager::GetCurrentRunScene()->GetActorManager();
    ActorObject* createActor = actorManager->FindActorByID(mCreateParentID);
    actorManager->DetachActor(createActor);
    mCreateParentActor = createActor;
    mIsActiveInScene = false;

    //もし生成したアクターが現在選択されていたら解除する
    Entity* selected = SelectionManager::GetSelectedActor();
    if (selected == createActor)
    {
        SelectionManager::SetSelectedActor(nullptr);
    }
}

void CreateActorFromFBXFileCommand::Redo() { Execute(); }
