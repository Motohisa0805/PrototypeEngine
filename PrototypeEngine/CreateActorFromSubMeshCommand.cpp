#include "CreateActorFromSubMeshCommand.h"
#include "MeshRenderer.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "AssetImporter.h"
#include "CreateActorFromFBXFileCommand.h"

CreateActorFromSubMeshCommand::CreateActorFromSubMeshCommand(
    const std::filesystem::path& assetPath, const string& localID,
    ActorObject* parentActor)
    : mTargetID(0)
    , mTarget(nullptr)
    , mAssetPath(assetPath)
    , mLocalID(localID)
    , mParentActor(parentActor)
    , mIsActiveInScene(false)
{
}

CreateActorFromSubMeshCommand::~CreateActorFromSubMeshCommand()
{
    if (!mIsActiveInScene && mTarget)
    {
        delete mTarget;
    }
}

void CreateActorFromSubMeshCommand::Execute() 
{
    ActorManager* actorManager =
        SceneManager::GetCurrentRunScene()->GetActorManager();

    if (mTargetID == 0)
    {
        // 1. 完全なる初回実行時：新しくアクターを生成してシーンに登録する
        mTarget   = new ActorObject();
        mTarget->SetName(mAssetPath.stem().filename().string());
        mTargetID = mTarget->GetID();
        AddComponent(mTarget);

        // シーンに所有権を渡したため、コマンド側のポインタは安全にクリアする
        mTarget          = nullptr;
        mIsActiveInScene = true;
    }
    else
    {
        // 2. Redo（再実行）時
        // Undo時にコマンド側（mTarget）に回収しておいたインスタンスを、もう一度シーンに戻す
        if (!mIsActiveInScene && mTarget)
        {
            actorManager->ReAddActor(dynamic_cast<ActorObject*>(mTarget));
            mTarget          = nullptr; // 所有権を再度シーンに渡す
            mIsActiveInScene = true;
        }
    }

    // 生成・復元された最新のオブジェクトをIDから解決して選択状態にする
    if (mTargetID != 0)
    {
        ActorObject* currentActor = actorManager->FindActorByID(mTargetID);
        if (currentActor)
        {
            SelectionManager::SetSelectedActor(currentActor);
        }
    }
}

void CreateActorFromSubMeshCommand::AddComponent(ActorObject* actor)
{
    if (mAssetPath.extension() == ".fbx")
    {
        MeshRenderer* mesh = new MeshRenderer(actor);
        mesh->LoadFilePathAndID(mAssetPath.string().c_str(),mLocalID.c_str());
        mesh->SetLocalID(mLocalID);
        actor->AddComponent(mesh);
    }
}

void CreateActorFromSubMeshCommand::Undo() 
{
    // 安全ガード
    if (mTargetID == 0 || !mIsActiveInScene)
        return;

    ActorManager* actorManager =
        SceneManager::GetCurrentRunScene()->GetActorManager();

    // 「その瞬間」にシーンに存在しているポインタをIDから検索
    ActorObject* currentActor = actorManager->FindActorByID(mTargetID);

    if (currentActor)
    {
        // シーンからアクターを除外
        actorManager->DetachActor(currentActor);

        mTarget = currentActor;
    }

    mIsActiveInScene = false;

    // もし現在生成したアクターが選択されていたら、安全に解除
    if (SelectionManager::GetSelectedActor() == currentActor ||
        SelectionManager::GetSelectedActor() == mTarget)
    {
        SelectionManager::SetSelectedActor(nullptr);
    }
}

void CreateActorFromSubMeshCommand::Redo() { Execute(); }
