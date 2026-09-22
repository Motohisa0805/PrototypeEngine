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

    bool isDataChanged = false;

    //ImGuiで直接値を編集
    ImGui::Text("State Settings");

    ImGui::Spacing();
    ImGui::Separator();
    //ループ処理設定
    bool isLoop = targetState->sAnimInfo.sIsLoop;
    if (ImGui::Checkbox("IsLoop", &isLoop))
    {
        targetState->sAnimInfo.sIsLoop = isLoop;
        isDataChanged                  = true;
    }

    ImGui::Separator();

    float speed = targetState->sPlaybackSpeed;
    if (ImGui::DragFloat("Speed", &speed, 0.01f, 0.0f, 10.0f))
    {
        targetState->sPlaybackSpeed = speed;
        isDataChanged               = true;
    }

    ImGui::Spacing();
    ImGui::Separator();

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

            isDataChanged = true;
        }
    }

    if (isDataChanged)
    {
        AnimatorControllerGenerater::GenerateController(animator->GetControllerFilePath(), controllerData);
        animator->LoadController(animator->GetControllerFilePath().string());
    }
}

void AnimationSettingsItem::DrawAnimTransitionSettings() 
{
    string selectedName = SelectionManager::GetSelectedTransitionName();
    if (selectedName.empty()) return;
    
    auto animator = AnimatorNodeEditorPanel::GetSelectAnimator();
    if (!animator) return;

    auto& controllerData = animator->GetControllerData();
    AnimTransition* targetTransition = nullptr;
    for (auto& trans : controllerData.sTransitions)
    {
        string transName = trans.sFromState + "->" + trans.sToState;
        if (transName == selectedName)
        {
            targetTransition = &trans;
            break;
        }
    }
    // 対象のステートが見つからなければ終了
    if (!targetTransition) return;

    bool isDataChanged = false;

    // ImGuiで直接値を編集
    ImGui::Text("Transition Settings");
    ImGui::Separator();
    //ブレンド時間の編集
    float blendDuration = targetTransition->sBlendDuration;
    if (ImGui::DragFloat("BlendDuration", &blendDuration, 0.01f, 0.0f, 10.0f))
    {
        targetTransition->sBlendDuration = blendDuration;
        isDataChanged                    = true;
    }

    ImGui::Spacing();
    ImGui::Separator();

    //遷移条件の編集
    //遷移条件ヘッダーと追加ボタン
    ImGui::Text("Conditions");
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20.0f);

    if (ImGui::Button("+##AddCondition", ImVec2(20, 20)))
    {
        if (!controllerData.sParameters.empty())
        {
            AnimCondition newCond;
            newCond.sParameterName = controllerData.sParameters[0].sName;

            if (controllerData.sParameters[0].sType == AnimParamType::sBool ||
                controllerData.sParameters[0].sType == AnimParamType::sTrigger)
            {
                newCond.sMode = "true";
            }
            else
            {
                newCond.sMode = "Greater";
            }
            newCond.sThreshold = 0.0f;

            targetTransition->sConditions.push_back(newCond);
            isDataChanged = true;
        }
    }

    //パラメーターが存在しない場合の警告表示
    if (controllerData.sParameters.empty())
    {
        ImGui::TextDisabled("No parameters registered");
    }
    else
    {
        int deleteIndex = -1;

        //各条件行の描画
        for (int i = 0; i < static_cast<int>(targetTransition->sConditions.size()); ++i)
        {
            auto& cond = targetTransition->sConditions[i];
            ImGui::PushID(i);

            //パラメーター選択(コンボボックス)
            ImGui::SetNextItemWidth(90.0f);
            if (ImGui::BeginCombo("##ParamCombo",cond.sParameterName.c_str()))
            {
                for (const auto& param : controllerData.sParameters)
                {
                    bool isSelected = (cond.sParameterName == param.sName);
                    if (ImGui::Selectable(param.sName.c_str(), isSelected))
                    {
                        cond.sParameterName = param.sName;

                        if (param.sType == AnimParamType::sBool || param.sType == AnimParamType::sTrigger)
                        {
                            cond.sMode = "true";
                        }
                        else
                        {
                            cond.sMode = "Greater";
                        }
                        isDataChanged = true;
                    }
                    if (isSelected)ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            // 現在選択されているパラメーターの型を取得
            AnimParamType currentType = AnimParamType::sFloat;
            for (const auto& param : controllerData.sParameters)
            {
                if (param.sName == cond.sParameterName)
                {
                    currentType = param.sType;
                    break;
                }
            }

            ImGui::SameLine();

            //比較モード
            if (currentType == AnimParamType::sBool || currentType == AnimParamType::sTrigger)
            {
                ImGui::SetNextItemWidth(95.0f);
                const char* modes[] = {"true", "false"};
                int         currentModeIdx = (cond.sMode == "false") ? 1 : 0;
                if (ImGui::Combo("##ModeCombo", &currentModeIdx, modes, IM_ARRAYSIZE(modes)))
                {
                    cond.sMode = modes[currentModeIdx];
                    isDataChanged = true;
                }
            }
            else
            {
                ImGui::SetNextItemWidth(95.0f);
                const char* modes[] = {"Greater", "Less", "Equals", "NotEquals"};
                int         currentModeIdx = 0;
                for (int m = 0; m < 4; ++m)
                {
                    if (cond.sMode == modes[m])
                    {
                        currentModeIdx = m;
                        break;
                    }
                }
                if (ImGui::Combo("##ModeCombo", &currentModeIdx, modes, IM_ARRAYSIZE(modes)))
                {
                    cond.sMode = modes[currentModeIdx];
                    isDataChanged = true;
                }

                ImGui::SameLine();

                ImGui::SetNextItemWidth(55.0f);
                if (ImGui::DragFloat("##Threshold", &cond.sThreshold, 0.1f))
                {
                    isDataChanged = true;
                }
            }

            ImGui::SameLine();

            //条件削除ボタン
            if (ImGui::Button("-##DeleteCond", ImVec2(20, 20)))
            {
                deleteIndex = i;
            }

            ImGui::PopID();
        }

        //削除実行
        if (deleteIndex != -1)
        {
            targetTransition->sConditions.erase(targetTransition->sConditions.begin() + deleteIndex);
            isDataChanged = true;
        }
    }
    //変更があった場合はJSON保存
    if (isDataChanged)
    {
        AnimatorControllerGenerater::GenerateController(
            animator->GetControllerFilePath(), controllerData);
        animator->LoadController(animator->GetControllerFilePath().string());
    }
}
