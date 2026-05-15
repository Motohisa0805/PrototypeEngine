#include "PasteActorCommand.h"

PasteActorCommand::PasteActorCommand()
	: mTarget(nullptr)
	, mIsActiveInScene(false)
{
}

PasteActorCommand::~PasteActorCommand()
{
    // Undoされたまま履歴から消えたら、コマンドが責任を持ってメモリを解放
    if (!mIsActiveInScene && mTarget)
    {
        delete mTarget;
    }
}

void PasteActorCommand::Execute()
{
    if (!mTarget)
    {
        // 初回実行時：クリップボードのアクターを「さらに複製」して生成
        if (EditorClipboard::HasCopiedActor())
        {
            mTarget = EditorClipboard::GetCopiedActor()->Clone();

            // コピーと区別がつくように(Copy)を追加
            mTarget->SetName(mTarget->GetName() + " (Copy)");
        }
    }
    else
    {
        // Redo時：既存のインスタンスを再利用してシーンに戻す
        SceneManager::GetNowScene()->GetActorManager()->AddActor(mTarget);
    }

    if (mTarget)
    {
        mIsActiveInScene = true;
        SelectionManager::SetSelectedActor(mTarget); // ペーストしたものを選択
    }

    //編集操作の変更を記録する
    string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
    SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
    EditorSettingsManager::SetSaveFlag(true);
}

void PasteActorCommand::Undo()
{
    if (!mTarget) return;
    SceneManager::GetNowScene()->GetActorManager()->RemoveActor(mTarget);
    mIsActiveInScene = false;

    if (SelectionManager::GetSelectedActor() == mTarget)
    {
        SelectionManager::SetSelectedActor(nullptr);
    }

    //編集操作の変更を記録する
    string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
    SceneSerializer::WriteEditorData(startupScenePath, SceneManager::GetNowScene());
    EditorSettingsManager::SetSaveFlag(true);
}

void PasteActorCommand::Redo()
{
    Execute();
}
