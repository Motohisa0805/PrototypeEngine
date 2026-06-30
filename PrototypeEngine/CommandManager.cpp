#include "CommandManager.h"

void CommandManager::Execute(std::unique_ptr<ICommand> command)
{
    command->Execute();

    // 新しい操作が行われたらRedoスタックはクリアする（Unityと同じ挙動）
    if (!GUIEditorManager::IsPlaying())
    {
        Get().mRedoStacks.clear();
        Get().mUndoStacks.push_back(std::move(command));

        if (Get().mUndoStacks.size() > 50)
        {
            Get().mUndoStacks.erase(
                Get().mUndoStacks.begin()); // 一番古いコマンドを削除
        }
        // 編集操作の変更を記録する
        string startupScenePath =
            EditorSettingsManager::GetInstance().GetLastOpenedScene();
        SceneSerializer::WriteEditingSceneData(
            startupScenePath, SceneManager::GetCurrentRunScene());
        EditorSettingsManager::SetSaveFlag(true);
    }
}

void CommandManager::NoHistoryExecute(std::unique_ptr<ICommand> command)
{
    command->NoHistoryExecute();

    // 新しい操作が行われたらRedoスタックはクリアする（Unityと同じ挙動）
    if (!GUIEditorManager::IsPlaying())
    {
        // 編集操作の変更を記録する
        string startupScenePath =
            EditorSettingsManager::GetInstance().GetLastOpenedScene();
        SceneSerializer::WriteEditingSceneData(
            startupScenePath, SceneManager::GetCurrentRunScene());
        EditorSettingsManager::SetSaveFlag(true);
    }
}

void CommandManager::Undo()
{
    if (GUIEditorManager::IsPlaying())
    {
        return;
    }
    if (Get().mUndoStacks.empty())
        return;

    auto command = std::move(Get().mUndoStacks.back());
    Get().mUndoStacks.pop_back();

    command->Undo();
    Get().mRedoStacks.push_back(std::move(command));

    // 編集操作の変更を記録する
    string startupScenePath =
        EditorSettingsManager::GetInstance().GetLastOpenedScene();
    SceneSerializer::WriteEditingSceneData(startupScenePath,
                                           SceneManager::GetCurrentRunScene());
    EditorSettingsManager::SetSaveFlag(true);
}

void CommandManager::Redo()
{
    if (GUIEditorManager::IsPlaying())
    {
        return;
    }
    if (Get().mRedoStacks.empty())
        return;

    auto command = std::move(Get().mRedoStacks.back());
    Get().mRedoStacks.pop_back();

    command->Execute();
    Get().mUndoStacks.push_back(std::move(command));

    // 編集操作の変更を記録する
    string startupScenePath =
        EditorSettingsManager::GetInstance().GetLastOpenedScene();
    SceneSerializer::WriteEditingSceneData(startupScenePath,
                                           SceneManager::GetCurrentRunScene());
    EditorSettingsManager::SetSaveFlag(true);
}

void CommandManager::Shutdown()
{
    Get().mUndoStacks.clear();
    Get().mRedoStacks.clear();
}

CommandManager& CommandManager::Get()
{
    static CommandManager instance;
    return instance;
}
