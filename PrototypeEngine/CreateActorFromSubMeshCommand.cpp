#include "CreateActorFromSubMeshCommand.h"
#include "MeshRenderer.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "AssetImporter.h"
#include "CreateActorTemplate.h"

CreateActorFromSubMeshCommand::CreateActorFromSubMeshCommand(
    const std::filesystem::path& assetPath, const string& localID,
    ActorObject* parentActor)
    : mCreateParentID(-1)
    , mCreateParentActor(nullptr) 
    , mAssetPath(assetPath)
    , mLocalID(localID)
    , mParentActor(parentActor)
    , mIsActiveInScene(false)
{
}

CreateActorFromSubMeshCommand::~CreateActorFromSubMeshCommand()
{
    if (mCreateParentActor && !mIsActiveInScene)
    {
        ReleasePasteActor(mCreateParentActor);
    }
}

void CreateActorFromSubMeshCommand::ReleasePasteActor(ActorObject* actor)
{
    for (ActorObject* child : actor->GetTransform()->GetChildActorList())
    {
        ReleasePasteActor(child);
    }
    delete actor;
}

void CreateActorFromSubMeshCommand::Execute() 
{
    ActorManager* actorManager =
        SceneManager::GetCurrentRunScene()->GetActorManager();

    if (mCreateParentID == -1)
    {
        // 1. 完全なる初回実行時：新しくアクターを生成してシーンに登録する
        CreateActorTemplate::CreateOneSubMeshActor(mCreateParentActor, mCreateParentID, mLocalID,mAssetPath);

        // シーンに所有権を渡したため、コマンド側のポインタは安全にクリアする
        mIsActiveInScene = true;
    }
    else
    {
        // 2. Redo（再実行）時
        // Undo時にコマンド側（mTarget）に回収しておいたインスタンスを、もう一度シーンに戻す
        if (!mIsActiveInScene && mCreateParentActor)
        {
            actorManager->ReAddActor(mCreateParentActor);
            mCreateParentActor = nullptr; // 所有権を再度シーンに渡す
            mIsActiveInScene = true;
        }
    }

    // 生成・復元された最新のオブジェクトをIDから解決して選択状態にする
    if (mCreateParentID != -1)
    {
        ActorObject* currentActor = actorManager->FindActorByID(mCreateParentID);
        if (currentActor)
        {
            SelectionManager::SetSelectedActor(currentActor);
        }
    }
}

void CreateActorFromSubMeshCommand::Undo() 
{
    // 安全ガード
    if (mCreateParentID == -1 || !mIsActiveInScene)
        return;

    ActorManager* actorManager =
        SceneManager::GetCurrentRunScene()->GetActorManager();

    // 「その瞬間」にシーンに存在しているポインタをIDから検索
    ActorObject* currentActor = actorManager->FindActorByID(mCreateParentID);

    if (currentActor)
    {
        // シーンからアクターを除外
        actorManager->DetachActor(currentActor);

        mCreateParentActor = currentActor;
    }

    mIsActiveInScene = false;

    // もし現在生成したアクターが選択されていたら、安全に解除
    if (SelectionManager::GetSelectedActor() == currentActor ||
        SelectionManager::GetSelectedActor() == mCreateParentActor)
    {
        SelectionManager::SetSelectedActor(nullptr);
    }
}

void CreateActorFromSubMeshCommand::Redo() { Execute(); }
