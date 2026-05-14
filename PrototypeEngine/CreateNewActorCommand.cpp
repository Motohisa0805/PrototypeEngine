#include "CreateNewActorCommand.h"

CreateNewActorCommand::CreateNewActorCommand()
	:mTarget(nullptr)
{
}

CreateNewActorCommand::~CreateNewActorCommand()
{
    // 【最重要】もし Undo された状態（シーンに存在しない状態）のまま、
    // コマンド履歴がクリアされたり、スタックから溢れてこのコマンドが破棄された場合、
    // 浮いた状態のアクターをここで安全に解放してメモリリークを防ぎます。
    if (!mIsActiveInScene && mTarget)
    {
        delete mTarget;
        mTarget = nullptr;
    }
}

void CreateNewActorCommand::Execute()
{
    if (!mTarget)
    {
        // 初回実行時：新しくアクターを生成
        mTarget = new ActorObject();
    }
    else
    {
        // Redo（再実行）時：すでに存在するインスタンスを再利用してシーンに戻す
        SceneManager::GetNowScene()->GetActorManager()->AddActor(mTarget);
    }

    mIsActiveInScene = true;

    // 生成したアクターを自動的に選択状態にする
    SelectionManager::SetSelectedActor(mTarget);

    //編集操作の変更を記録する
    string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
    SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
    EditorSettingsManager::SetSaveFlag(true);
}

void CreateNewActorCommand::Undo()
{
    if (!mTarget)return;
    //シーンのメインリストからアクターを完全に除外
    //deleteせずmTargetに残す
    SceneManager::GetNowScene()->GetActorManager()->RemoveActor(mTarget);

    mIsActiveInScene = false;

    //もし現在生成したアクターが選択されていたら、解除
    if (SelectionManager::GetSelectedActor() == mTarget) {
        SelectionManager::SetSelectedActor(nullptr);
    }

    //編集操作の変更を記録する
    string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
    SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
    EditorSettingsManager::SetSaveFlag(true);
}

void CreateNewActorCommand::Redo()
{
	Execute();
}
