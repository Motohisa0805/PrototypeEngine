#include "CreateActorFromFBXFileCommand.h"
#include "MeshRenderer.h"
#include "AssetImporter.h"
#include "SceneManager.h"
#include <filesystem>
#include <nlohmann/json.hpp>
#include "DebugManager.h"

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
        std::filesystem::path metaPath = AssetImporter::GeneratedMetaFilePath(mAssetPath);
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

        //再帰的にアクターを生成し、親子関係を構築するラムダ
        auto CreateHierarchy = [&](auto& self, const nlohmann::json& nodeJson, ActorObject* currentParent) -> void
        {
            //アクターの生成と名前設定
            ActorObject* newActor = new ActorObject();
            newActor->SetName(nodeJson.value("name", "UnnamedNode"));

            // 親子関係の設定
            if (currentParent)
            {
                newActor->GetTransform()->SetParent(currentParent);
            }

            //Transformの初期化
            if (nodeJson.contains("transform"))
            {
                auto t = nodeJson["transform"];
                newActor->GetTransform()->SetPosition(Vector3(t[0], t[1], t[2]));
            }
            if (nodeJson.contains("rotation"))
            {
                auto r = nodeJson["rotation"];
                newActor->GetTransform()->SetRotation(Quaternion(r[0], r[1], r[2],r[3]));
            }
            if (nodeJson.contains("scale"))
            {
                auto s = nodeJson["scale"];
                newActor->GetTransform()->SetScale(Vector3(s[0], s[1], s[2]));
            }

            //メッシュのアタッチ
            if (nodeJson.contains("mesh_indices"))
            {
                for (const auto& idJson : nodeJson["mesh_indices"])
                {
                    string localID = idJson.get<string>();
                    MeshRenderer* mesh    = new MeshRenderer(newActor);
                    mesh->LoadFilePathAndID(mAssetPath.string().c_str(),localID.c_str());
                    mesh->SetLocalID(localID);
                    newActor->AddComponent(mesh);
                }
            }

            mTargetIDs.push_back(newActor->GetID());

            //子ノードの再起処理
            if (nodeJson.contains("children"))
            {
                for (const auto& childJson : nodeJson["children"])
                {
                    self(self, childJson, newActor);
                }
            }
        };

        CreateHierarchy(CreateHierarchy, hierarchyJson, mParentActor);

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
