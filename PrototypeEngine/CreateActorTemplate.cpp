#include "CreateActorTemplate.h"
#include "MeshRenderer.h"
#include "SkeletalMeshRenderer.h"
#include "Animator.h"

bool CreateActorTemplate::CreateOneSubMeshActor(ActorObject* target,uint64_t& id,const string& localID,filesystem::path path)
{
    //オブジェクトの生成
    target = new ActorObject();
    target->SetName(path.stem().filename().string());
    id = target->GetID();
    //コンポーネントの追加
    MeshRenderer* mesh = new MeshRenderer(target);
    mesh->LoadFilePathAndID(path.string().c_str(), localID.c_str());
    mesh->SetLocalID(localID);
    target->AddComponent(mesh);
    return true;
}

bool CreateActorTemplate::CreateFBXFileActor(const nlohmann::json& nodeJson,ActorObject* currentParent,filesystem::path path,vector<uint64_t>& targetIDs)
{
    ActorObject* newActor = new ActorObject();
    newActor->SetName(nodeJson.value("name", "UnnamedNode"));
    //親子関係の設定
    if (currentParent)
    {
        newActor->GetTransform()->SetParent(currentParent);
    }

    // Transformの初期化
    if (nodeJson.contains("transform"))
    {
        auto t = nodeJson["transform"];
        newActor->GetTransform()->SetLocalPosition(Vector3(t[0], t[1], t[2]));
    }
    if (nodeJson.contains("rotation"))
    {
        auto r = nodeJson["rotation"];
        newActor->GetTransform()->SetLocalRotation(
            Quaternion(r[0], r[1], r[2], r[3]));
    }
    if (nodeJson.contains("scale"))
    {
        auto s = nodeJson["scale"];
        newActor->GetTransform()->SetLocalScale(Vector3(s[0], s[1], s[2]));
    }

    // メッシュのアタッチ
    if (nodeJson.contains("mesh_indices"))
    {
        for (const auto& idJson : nodeJson["mesh_indices"])
        {
            string        localID = idJson.get<string>();
            MeshRenderer* mesh    = new MeshRenderer(newActor);
            mesh->LoadFilePathAndID(path.string().c_str(),
                                    localID.c_str());
            mesh->SetLocalID(localID);
            newActor->AddComponent(mesh);
        }
    }

    targetIDs.push_back(newActor->GetID());

    // 子ノードの再起処理
    if (nodeJson.contains("children"))
    {
        for (const auto& childJson : nodeJson["children"])
        {
            CreateFBXFileActor(childJson, newActor, path, targetIDs);
        }
    }

    return true;
}

bool CreateActorTemplate::CreateSkinnedMeshActor(const nlohmann::json& nodeJson,
                                                 ActorObject*     currentParent,
                                                 filesystem::path path,
                                                 vector<uint64_t>& targetIDs)
{
    ActorObject* newActor = new ActorObject();
    newActor->SetName(nodeJson.value("name", "UnnamedNode"));
    // 親子関係の設定
    if (currentParent)
    {
        newActor->GetTransform()->SetParent(currentParent);
    }

    // Transformの初期化
    if (nodeJson.contains("transform"))
    {
        auto t = nodeJson["transform"];
        newActor->GetTransform()->SetLocalPosition(Vector3(t[0], t[1], t[2]));
    }
    if (nodeJson.contains("rotation"))
    {
        auto r = nodeJson["rotation"];
        newActor->GetTransform()->SetLocalRotation(
            Quaternion(r[0], r[1], r[2], r[3]));
    }
    if (nodeJson.contains("scale"))
    {
        auto s = nodeJson["scale"];
        newActor->GetTransform()->SetLocalScale(Vector3(s[0], s[1], s[2]));
    }

    // メッシュのアタッチ
    if (nodeJson.contains("mesh_indices"))
    {
        for (const auto& idJson : nodeJson["mesh_indices"])
        {
            string        localID = idJson.get<string>();
            SkeletalMeshRenderer* mesh    = new SkeletalMeshRenderer(newActor);
            mesh->LoadSkeletonMesh(path.string().c_str(), localID.c_str(), currentParent);
            mesh->SetLocalID(localID);
            newActor->AddComponent(mesh);
        }
    }

    targetIDs.push_back(newActor->GetID());

    // 子ノードの再起処理
    if (nodeJson.contains("children"))
    {
        for (const auto& childJson : nodeJson["children"])
        {
            CreateSkinnedMeshActor(childJson, newActor, path, targetIDs);
        }
    }

    return true;
}

bool CreateActorTemplate::CreateSkeletonActor(const nlohmann::json& metaJson,const nlohmann::json& nodeJson,
                                              ActorObject*      currentParent,filesystem::path  path,
                                              vector<uint64_t>& targetIDs)
{
    //親オブジェクトはファイルを元に名前を設定
    ActorObject* newActor = new ActorObject();
    newActor->SetName(nodeJson.value("name", "UnnamedNode"));
    // 親子関係の設定
    if (currentParent)
    {
        newActor->GetTransform()->SetParent(currentParent);
    }
    //親オブジェクトにはAnimatorをアタッチ
    Animator* animator = new Animator(newActor);
    newActor->AddComponent(animator);
    //親オブジェクトのIDを追加
    targetIDs.push_back(newActor->GetID());

    //ここからサブメッシュ、ボーンのヒエラルキー順にオブジェクトの親子関係を構築
    //サブメッシュを数分生成
    //  子ノードの再起処理
    if (nodeJson.contains("children"))
    {
        for (const auto& childJson : nodeJson["children"])
        {
            CreateSkinnedMeshActor(childJson, newActor, path, targetIDs);
        }
    }
    // AnimatorにSkeletonDataをロード
    animator->LoadSkeletonData(path.string().c_str(), newActor);
    // アニメーションのロード
    if (metaJson.contains("cached_data") && metaJson["cached_data"].contains("animations"))
    {
        for (const auto& animJson : metaJson["cached_data"]["animations"])
        {
            filesystem::path binaryPath = animJson.value("binary_path", "");
            //animator->Load(binaryPath.string().c_str());
            //animator->Load("Idle_anim0.animbin");
        }
    }


    return true;
}
