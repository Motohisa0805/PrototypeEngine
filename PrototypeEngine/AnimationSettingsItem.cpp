#include "AnimationSettingsItem.h"
#include "AnimatorNodeEditorPanel.h"
#include "SelectionManager.h"
#include "AnimatorControllerGenerater.h"

void AnimationSettingsItem::DrawAnimStateSettings() 
{
    string selectedName = SelectionManager::GetSelectedStateName();
    if (selectedName.empty())return;

    auto animator = AnimatorNodeEditorPanel::GetSelectAnimator();
    if (!animator)return;

    auto& controllerData = animator->GetControllerData();

    AnimState* targetState = nullptr;
    for (auto& state : controllerData.sStates)
    {
        if (state.sStateName == selectedName)
        {
            targetState = &state;
            break;
        }
    }
    //対象のステートが見つからなければ終了
    if (!targetState)return;
    //ImGuiで直接値を編集
    ImGui::Text("State Settings");
    ImGui::Separator();
    float speed = targetState->sPlaybackSpeed;
    if (ImGui::DragFloat("Speed", &speed, 0.01f, 0.0f, 10.0f))
    {
        targetState->sPlaybackSpeed = speed;
    }

    //ステート名の変更
    char buffer[128];
    strncpy_s(buffer, targetState->sStateName.c_str(), sizeof(buffer));
    if (ImGui::InputText("State Name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        string newName = buffer;
        if (!newName.empty() && newName != targetState->sStateName)
        {
            for (auto& trans : controllerData.sTransitions)
            {
                if (trans.sFromState == targetState->sStateName)trans.sFromState = newName;
                if (trans.sToState == targetState->sStateName)trans.sToState = newName;
            }
            if (controllerData.sDefaultState == targetState->sStateName)
            {
                controllerData.sDefaultState = newName;
            }

            targetState->sStateName = newName;
            SelectionManager::SetSelectedStateName(newName);

            AnimatorControllerGenerater::GenerateController(animator->GetControllerFilePath(), controllerData);
            animator->LoadController(animator->GetControllerFilePath().string());
        }
    }
}
