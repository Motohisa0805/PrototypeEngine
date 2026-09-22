#include "AnimatorNodeEditorPanel.h"
#include "GUIEditorManager.h"
#include "AssetDataBase.h"
#include "AnimatorControllerGenerater.h"
#include "SelectionManager.h"
#include "ProjectPanel.h"

Animator* AnimatorNodeEditorPanel::mSelectAnimator             = nullptr;

Animator* AnimatorNodeEditorPanel::mPreviousAnimator             = nullptr;

AnimatorNodeEditorPanel::AnimatorNodeEditorPanel(Renderer* renderer)
    : EditorWindow(renderer)
    , mNeedSetNodePositions(false)
    , mIsTransitionUIFrag(false)
{
    mID = "Animator Controller Editor";
}

void AnimatorNodeEditorPanel::Initialize(float width, float height,
                                         ImTextureRef ref)
{
}

void AnimatorNodeEditorPanel::ParameterDraw() 
{
    if (!mSelectAnimator)
    {
        ImGui::TextDisabled("No Animator Selected");
        return;
    }

    auto& controllerData = mSelectAnimator->GetControllerData();
    bool  isDataChanged  = false;

    //ヘッダーと追加ボタン
    ImGui::TextUnformatted("Parameters");
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20.0f);
    if (ImGui::Button("+", ImVec2(20, 20)))
    {
        ImGui::OpenPopup("AddParameterPopup");
    }

    if (ImGui::BeginPopup("AddParameterPopup"))
    {
        if (ImGui::MenuItem("Float"))
        {
            AnimParameter param;
            param.sName         = "New Float";
            param.sType         = AnimParamType::sFloat;
            param.sDefaultFloat = 0.0f;
            controllerData.sParameters.push_back(param);
            isDataChanged = true;
        }
        if (ImGui::MenuItem("Int"))
        {
            AnimParameter param;
            param.sName         = "New Int";
            param.sType         = AnimParamType::sInt;
            param.sDefaultInt   = 0;
            controllerData.sParameters.push_back(param);
            isDataChanged = true;
        }
        if (ImGui::MenuItem("Bool"))
        {
            AnimParameter param;
            param.sName          = "New Bool";
            param.sType          = AnimParamType::sBool;
            param.sDefaultBool   = false;
            controllerData.sParameters.push_back(param);
            isDataChanged = true;
        }
        if (ImGui::MenuItem("Trigger"))
        {
            AnimParameter param;
            param.sName             = "New Trigger";
            param.sType             = AnimParamType::sTrigger;
            param.sDefaultTrigger   = false;
            controllerData.sParameters.push_back(param);
            isDataChanged = true;
        }
        ImGui::EndPopup();
    }
    ImGui::Separator();

    int deleteIndex = -1;
    for (int i = 0; i < static_cast<int>(controllerData.sParameters.size()); ++i)
    {
        auto& param = controllerData.sParameters[i];
        ImGui::PushID(i);

        char nameBuffer[64];
        strncpy_s(nameBuffer, param.sName.c_str(), sizeof(nameBuffer));
        ImGui::SetNextItemWidth(90.0f);
        if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            param.sName = nameBuffer;
            isDataChanged = true;
        }

        ImGui::SameLine();

        //パラメーター型に応じて値編集コントロール
        ImGui::SetNextItemWidth(60.0f);
        switch (param.sType)
        {
        case AnimParamType::sFloat:
            if (ImGui::DragFloat("##Value", &param.sDefaultFloat, 0.1f))
            {
                isDataChanged = true;
            }
            break;
        case AnimParamType::sInt:
        {
            int intVal = param.sDefaultInt;
            if (ImGui::DragInt("##Value", &intVal, 1.0f))
            {
                param.sDefaultInt = intVal;
                isDataChanged     = true;
            }
        }
            break;
        case AnimParamType::sBool:
            if (ImGui::Checkbox("##Value", &param.sDefaultBool))
            {
                isDataChanged = true;
            }
            break;
        case AnimParamType::sTrigger:
            if (ImGui::Checkbox("##Trigger", &param.sDefaultTrigger))
            {
                isDataChanged = true;
            }
            break;
        }

        ImGui::SameLine();

        if (ImGui::Button("x##Delete", ImVec2(20, 20)))
        {
            deleteIndex = i;
        }
        ImGui::PopID();
    }

    //削除処理
    if (deleteIndex != -1)
    {
        controllerData.sParameters.erase(controllerData.sParameters.begin() + deleteIndex);
        isDataChanged = true;
    }

    if (isDataChanged)
    {
        AnimatorControllerGenerater::GenerateController(
            mSelectAnimator->GetControllerFilePath(), controllerData);
        mSelectAnimator->LoadController(
            mSelectAnimator->GetControllerFilePath().string());
    }
}

void AnimatorNodeEditorPanel::Draw(float width, float height) 
{
    if (ImGui::Begin(mID.c_str()))
    {
        //左側エリア
        float leftWidth = 220.0f;
        ImGui::BeginChild("ParameterPanel", ImVec2(leftWidth, 0.0f), true);
        ParameterDraw();
        ImGui::EndChild();
        ImGui::SameLine();
        //右側エリア
        ImGui::BeginChild("NodeEditorPanel", ImVec2(0.0f, 0.0f), true);
        // デバッグモード切り替えボタン
        //ImGuiHelper::FragTextButton("O", ImVec2(0.0f, 0.0f),mIsTransitionUIFrag,false);

        ne::SetCurrentEditor(GUIEditorManager::GetNodeContext());
        ne::Begin("Animator Controller Editor");
        std::unordered_map<uint64_t, int> linkToTransIndexMap;
        if (mSelectAnimator && mSelectAnimator->GetOwner())
        {
            auto& controllerData = mSelectAnimator->GetControllerData();

            if (mSelectAnimator != mPreviousAnimator)
            {
                mNeedSetNodePositions = true;
                mPreviousAnimator     = mSelectAnimator;
            }

            // ステート名からピンIDを引くためのマップ
            std::unordered_map<string, uint64_t> inputPinMap;
            std::unordered_map<string, uint64_t> outputPinMap;
            std::unordered_map<uint64_t, string> pinToStateMap; // 新規作成時の判定用

            float normalizedTime = mSelectAnimator->GetNormalizedTime();

            int nodeIndex = 1;
            for (auto& state : controllerData.sStates)
            {
                ne::NodeId nodeId = nodeIndex;
                // 1ノードあたり固定の範囲でPinIDを振り分ける
                ne::PinId inPinId  = nodeIndex * 100 + 1;
                ne::PinId outPinId = nodeIndex * 100 + 2;

                inputPinMap[state.sStateName]  = inPinId.Get();
                outputPinMap[state.sStateName] = outPinId.Get();
                pinToStateMap[inPinId.Get()]   = state.sStateName;
                pinToStateMap[outPinId.Get()]  = state.sStateName;

                if (mNeedSetNodePositions)
                {
                    ne::SetNodePosition(nodeId,
                                        ImVec2(state.sPos.x, state.sPos.y));
                }

                const float nodeWidth = 160.0f;

                ne::BeginNode(nodeId);

                ImGui::Dummy(ImVec2(nodeWidth, 0.0f));

                float textWidth = ImGui::CalcTextSize(state.sStateName.c_str()).x;
                if (textWidth < nodeWidth)
                {
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (nodeWidth - textWidth) * 0.5f);
                }
                ImGui::Text("%s", state.sStateName.c_str());

                if (state.sStateName == mSelectAnimator->GetCurrentStateName())
                {
                    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.18f,0.52f,0.88f,1.0f));
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f,0.1f,0.1f,0.5f));

                    ImGui::ProgressBar(normalizedTime, ImVec2(120.0f, 4.0f), "");

                    ImGui::PopStyleColor(2);
                }

                bool hasInput = (state.sNodeType != AnimNodeType::sEntry && state.sNodeType != AnimNodeType::sAnyState);
                bool hasOutput = (state.sNodeType != AnimNodeType::sExit);

                if (hasInput)
                {
                    ne::BeginPin(inPinId, ne::PinKind::Input);
                    ImGui::Text("-> In");
                    ne::EndPin();
                    ImGui::SameLine();
                }

                if (hasOutput)
                {
                    if (hasInput)
                    {
                        ImGui::SameLine();
                        float outTextWidth = ImGui::CalcTextSize("Out ->").x;
                        ImGui::SetCursorPosX(ne::GetNodePosition(nodeId).x + nodeWidth - outTextWidth);
                    }
                    ne::BeginPin(outPinId, ne::PinKind::Output);
                    ImGui::Text("Out ->");
                    ne::EndPin();
                }
                ne::EndNode();

                ImVec2 currentPos = ne::GetNodePosition(nodeId);
                state.sPos        = Vector2(currentPos.x, currentPos.y);

                nodeIndex++;
            }

            mNeedSetNodePositions = false;

            int linkID = 1;
            for (size_t i = 0; i < controllerData.sTransitions.size(); ++i)
            {
                const auto& trans = controllerData.sTransitions[i];
                if (outputPinMap.count(trans.sFromState) &&
                    inputPinMap.count(trans.sToState))
                {
                    ne::PinId startPin = outputPinMap[trans.sFromState];
                    ne::PinId endPin   = inputPinMap[trans.sToState];
                    
                    ne::LinkId id = linkID++;
                    ne::Link(id, startPin, endPin);
                    //常時矢印を表示
                    ne::Flow(id);
                    //再生中のステート遷移のみ流す処理(現在は停止)
                    //if (mSelectAnimator->GetCurrentStateName() == trans.sFromState)
                    //{
                    //    ne::Flow(id);
                    //}
                    //LinkId(Get())とtransitionsの配列インデックスを紐づけ
                    linkToTransIndexMap[id.Get()] = static_cast<int>(i);
                }
            }

            // ユーザー操作による新規Linkの作成
            if (ne::BeginCreate())
            {
                ne::PinId startPinId, endPinId;
                if (ne::QueryNewLink(&startPinId, &endPinId))
                {
                    if (startPinId && endPinId)
                    {
                        if (ne::AcceptNewItem())
                        {
                            string fromState = pinToStateMap[startPinId.Get()];
                            string toState   = pinToStateMap[endPinId.Get()];

                            if (!fromState.empty() && !toState.empty() &&
                                fromState != toState)
                            {
                                AnimTransition newTrans;
                                newTrans.sFromState     = fromState;
                                newTrans.sToState       = toState;
                                newTrans.sBlendDuration = 0.25f;

                                controllerData.sTransitions.push_back(newTrans);

                                // JSONへの書き出しと再ロード
                                AnimatorControllerGenerater::GenerateController(
                                    mSelectAnimator->GetControllerFilePath(),
                                    controllerData);
                                mSelectAnimator->LoadController(
                                    mSelectAnimator->GetControllerFilePath()
                                        .string());
                            }
                        }
                    }
                }
            }
            ne::EndCreate();

            // 削除処理(選択してDeleteキー)
            if (ne::BeginDelete())
            {
                // ノードの削除処理
                ne::NodeId deletedNodeId;
                while (ne::QueryDeletedNode(&deletedNodeId))
                {
                    if (ne::AcceptDeletedItem())
                    {
                        int   index = static_cast<int>(deletedNodeId.Get()) - 1;
                        auto& states = controllerData.sStates;
                        if (index >= 0 && index < states.size())
                        {
                            const auto& state = states[index];

                            if (state.sNodeType != AnimNodeType::sEntry &&
                                state.sNodeType != AnimNodeType::sExit &&
                                state.sNodeType != AnimNodeType::sAnyState)
                            {
                                DeletedNode(index, controllerData);
                            }
                        }
                    }
                }

                // リンクの削除処理
                ne::LinkId deletedLinkId;
                while (ne::QueryDeletedLink(&deletedLinkId))
                {
                    if (ne::AcceptDeletedItem())
                    {
                        int   index = static_cast<int>(deletedLinkId.Get()) - 1;
                        auto& transitions = controllerData.sTransitions;
                        if (index >= 0 && index < transitions.size())
                        {
                            transitions.erase(transitions.begin() + index);

                            // JSONへの書き出しと再ロード
                            AnimatorControllerGenerater::GenerateController(
                                mSelectAnimator->GetControllerFilePath(),
                                controllerData);
                            mSelectAnimator->LoadController(
                                mSelectAnimator->GetControllerFilePath()
                                    .string());
                        }
                    }
                }
            }
            ne::EndDelete();

            // 右クリックメニュー
            ne::Suspend();
            static ne::NodeId contextNodeId = 0;
            if (ne::ShowNodeContextMenu(&contextNodeId))
            {
                ImGui::OpenPopup("NodeContextMenu");
            }

            if (ImGui::BeginPopup("NodeContextMenu"))
            {
                int   index  = static_cast<int>(contextNodeId.Get()) - 1;
                auto& states = controllerData.sStates;

                if (index >= 0 && index < states.size())
                {
                    auto& state = states[index];

                    if (state.sNodeType != AnimNodeType::sEntry &&
                        state.sNodeType != AnimNodeType::sExit &&
                        state.sNodeType != AnimNodeType::sAnyState)
                    {
                        if (ImGui::MenuItem("Set as Default State"))
                        {
                            controllerData.sDefaultState = state.sStateName;

                            // JSONへの書き出しと再ロード
                            AnimatorControllerGenerater::GenerateController(
                                mSelectAnimator->GetControllerFilePath(),
                                controllerData);
                            mSelectAnimator->LoadController(
                                mSelectAnimator->GetControllerFilePath()
                                    .string());
                        }
                        if (ImGui::MenuItem("Delete"))
                        {
                            DeletedNode(index, controllerData);
                        }
                    }
                }
                ImGui::EndPopup();
            }
            ne::Resume();
        }
        ne::End();

        //画面クリック処理
        if (ne::IsBackgroundClicked())
        {
            SelectionManager::ClearStateSelection();
            SelectionManager::ClearTransitionSelection();
        }
        //選択状態の変更(ノードやリンクをクリックして選択した時)
        if (mSelectAnimator && ne::HasSelectionChanged())
        {
            int selectedCount = ne::GetSelectedObjectCount();
            if (selectedCount > 0)
            {
                vector<ne::NodeId> selectedNodes(selectedCount);
                int nodeCount = ne::GetSelectedNodes(selectedNodes.data(),selectedCount);

                if (nodeCount > 0)
                {
                    int index = static_cast<int>(selectedNodes[0].Get()) - 1;
                    if (index >= 0 && index < mSelectAnimator->GetControllerData().sStates.size())
                    {
                        auto& selectedState = mSelectAnimator->GetControllerData().sStates[index];
                        SelectionManager::SetSelectedStateName(selectedState.sStateName);
                    }
                }
                else
                {
                    //接続線(リンク)選択の判定
                    vector<ne::LinkId> selectedLinks(selectedCount);
                    int                linkCount = ne::GetSelectedLinks(selectedLinks.data(),selectedCount);
                    if (linkCount > 0)
                    {
                        uint64_t selectedLinkId = selectedLinks[0].Get();
                        if (linkToTransIndexMap.count(selectedLinkId))
                        {
                            int transIndex = linkToTransIndexMap[selectedLinkId];
                            const auto& trans =
                                mSelectAnimator->GetControllerData().sTransitions[transIndex];

                            string transName = trans.sFromState + "->" + trans.sToState;
                            SelectionManager::SetSelectedTransitionName(transName);
                        }
                    }
                }
            }
        }
        //特定のノードをダブルクリックした時の判定
        ne::NodeId doubleClickedNodeId = ne::GetDoubleClickedNode();
        if (doubleClickedNodeId.Get() != 0)
        {
            int index = static_cast<int>(doubleClickedNodeId.Get()) - 1;
            if (index >= 0 && index < mSelectAnimator->GetControllerData().sStates.size())
            {
                const auto& targetState = mSelectAnimator->GetControllerData().sStates[index];
                if (!targetState.sAnimInfo.sBinayPath.empty())
                {
                    ProjectPanel::SetSelectedFolderPath(targetState.sAnimInfo.sBinayPath);
                }
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();

    //ドラッグ&ドロップの受け取り処理
    if (mSelectAnimator && ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ANIM_ITEM"))
        {
            IM_ASSERT(payload->DataSize == sizeof(AnimPayload));

            const AnimPayload* dropData = (const AnimPayload*)payload->Data;

            ImVec2 dropPos = ne::ScreenToCanvas(ImGui::GetMousePos());

            filesystem::path animPath = dropData->sAnimBinaryPath;

            AnimState newState;
            newState.sStateName = dropData->sAnimDataName;
            newState.sAnimInfo.sBinayPath = animPath;
            newState.sPlaybackSpeed  = 1.0f;

            mSelectAnimator->GetControllerData().sStates.push_back(newState);

            AnimatorControllerGenerater::GenerateController(
                mSelectAnimator->GetControllerFilePath(),
                mSelectAnimator->GetControllerData()
            );

            mSelectAnimator->LoadController(mSelectAnimator->GetControllerFilePath().string());
        }
        ImGui::EndDragDropTarget();
    }
}

void AnimatorNodeEditorPanel::DeletedNode(int index,AnimatorControllerParameters& controllerData)
{
    if (index < 0 || index >= controllerData.sStates.size())return;

    string deletedStateName = controllerData.sStates[index].sStateName;
    // 削除対象のStateに関連するTransition(遷移)を全て削除
    auto& transitions = controllerData.sTransitions;
    transitions.erase(
        std::remove_if(transitions.begin(), transitions.end(),
                       [&](const AnimTransition& trans)
                       {
                           return trans.sFromState == deletedStateName ||
                                  trans.sToState == deletedStateName;
                       }),
        transitions.end());
    // もし削除対象がDefaultStateだった場合はクリア
    if (controllerData.sDefaultState == deletedStateName)
    {
        controllerData.sDefaultState.clear();
    }
    // indexを直接使ってState配列から削除
    controllerData.sStates.erase(controllerData.sStates.begin() + index);

    AnimatorControllerGenerater::GenerateController(
        mSelectAnimator->GetControllerFilePath(), controllerData);
    mSelectAnimator->LoadController(
        mSelectAnimator->GetControllerFilePath().string());
}
