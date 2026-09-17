#include "AnimatorNodeEditorPanel.h"
#include "GUIEditorManager.h"
#include "AssetDataBase.h"
#include "AnimatorControllerGenerater.h"

Animator* AnimatorNodeEditorPanel::mSelectAnimator             = nullptr;

Animator* AnimatorNodeEditorPanel::mPreviousAnimator             = nullptr;

AnimatorNodeEditorPanel::AnimatorNodeEditorPanel(Renderer* renderer)
    : EditorWindow(renderer)
    , mNeedSetNodePositions(false)
{
    mID = "Animator Controller Editor";
}

void AnimatorNodeEditorPanel::Initialize(float width, float height,
                                         ImTextureRef ref)
{
}

void AnimatorNodeEditorPanel::Draw(float width, float height) 
{
    ne::SetCurrentEditor(GUIEditorManager::GetNodeContext());
    ne::Begin("Animator Controller Editor");

    if (mSelectAnimator)
    {
        if (mSelectAnimator != mPreviousAnimator)
        {
            mNeedSetNodePositions = true;
            mPreviousAnimator     = mSelectAnimator;
        }

        //ステート名からピンIDを引くためのマップ
        std::unordered_map<string, uint64_t> inputPinMap;
        std::unordered_map<string, uint64_t> outputPinMap;
        std::unordered_map<uint64_t, string> pinToStateMap; // 新規作成時の判定用

        int nodeIndex = 1;
        for (auto& state : mSelectAnimator->GetControllerData().sStates)
        {
            ne::NodeId nodeId = nodeIndex;
            //1ノードあたり固定の範囲でPinIDを振り分ける
            ne::PinId inPinId = nodeIndex * 100 + 1;
            ne::PinId outPinId = nodeIndex * 100 + 2;

            inputPinMap[state.sStateName]  = inPinId.Get();
            outputPinMap[state.sStateName] = outPinId.Get();
            pinToStateMap[inPinId.Get()]   = state.sStateName;
            pinToStateMap[outPinId.Get()]  = state.sStateName;

            if (mNeedSetNodePositions)
            {
                ne::SetNodePosition(nodeId, ImVec2(state.sPos.x, state.sPos.y));
            }

            ne::BeginNode(nodeId);
                ImGui::Text("%s", state.sStateName.c_str());

                if (state.sNodeType != AnimNodeType::sEntry && state.sNodeType != AnimNodeType::sAnyState)
                {
                    ne::BeginPin(inPinId, ne::PinKind::Input);
                    ImGui::Text("-> In");
                    ne::EndPin();
                    ImGui::SameLine();
                }

                if (state.sNodeType != AnimNodeType::sExit)
                {
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
        for (const auto& trans : mSelectAnimator->GetControllerData().sTransitions)
        {
            if (outputPinMap.count(trans.sFromState) && inputPinMap.count(trans.sToState))
            {
                ne::PinId startPin = outputPinMap[trans.sFromState];
                ne::PinId endPin   = inputPinMap[trans.sToState];
                ne::Link(linkID++, startPin, endPin);
            }
        }

        //ユーザー操作による新規Linkの作成
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

                        if (!fromState.empty() && !toState.empty() && fromState != toState)
                        {
                            AnimTransition newTrans;
                            newTrans.sFromState = fromState;
                            newTrans.sToState   = toState;
                            newTrans.sBlendDuration = 0.25f;

                            mSelectAnimator->GetControllerData().sTransitions.push_back(newTrans);

                            //JSONへの書き出しと再ロード
                            AnimatorControllerGenerater::GenerateController(
                                mSelectAnimator->GetControllerFilePath(),
                                mSelectAnimator->GetControllerData()
                            );
                            mSelectAnimator->LoadController(mSelectAnimator->GetControllerFilePath().string());
                        }
                    }
                }
            }
        }
        ne::EndCreate();

        //Linkの削除処理(選択してDeleteキー)
        if (ne::BeginDelete())
        {
            ne::LinkId deletedLinkId;
            while (ne::QueryDeletedLink(&deletedLinkId))
            {
                if (ne::AcceptDeletedItem())
                {
                    int index = static_cast<int>(deletedLinkId.Get()) - 1;
                    auto& transitions = mSelectAnimator->GetControllerData().sTransitions;
                    if (index >= 0 && index < transitions.size())
                    {
                        transitions.erase(transitions.begin() + index);

                        // JSONへの書き出しと再ロード
                        AnimatorControllerGenerater::GenerateController(
                            mSelectAnimator->GetControllerFilePath(),
                            mSelectAnimator->GetControllerData()
                        );
                        mSelectAnimator->LoadController(mSelectAnimator->GetControllerFilePath().string());
                    }
                }
            }
        }
        ne::EndDelete();

        //右クリックメニュー
        ne::Suspend();
        static ne::NodeId contextNodeId = 0;
        if (ne::ShowNodeContextMenu(&contextNodeId))
        {
            ImGui::OpenPopup("NodeContextMenu");
        }

        if (ImGui::BeginPopup("NodeContextMenu"))
        {
            int index = static_cast<int>(contextNodeId.Get()) - 1;
            auto& states = mSelectAnimator->GetControllerData().sStates;

            if (index >= 0 && index < states.size())
            {
                auto& state = states[index];

                if (state.sNodeType != AnimNodeType::sEntry &&
                    state.sNodeType != AnimNodeType::sExit &&
                    state.sNodeType != AnimNodeType::sAnyState)
                {
                    if (ImGui::MenuItem("Set as Default State"))
                    {
                        mSelectAnimator->GetControllerData().sDefaultState = state.sStateName;

                        // JSONへの書き出しと再ロード
                        AnimatorControllerGenerater::GenerateController(
                            mSelectAnimator->GetControllerFilePath(),
                            mSelectAnimator->GetControllerData());
                        mSelectAnimator->LoadController(mSelectAnimator->GetControllerFilePath().string());
                    }
                }
            }

            ImGui::EndPopup();
        }
        ne::Resume();
    }


    ne::End();

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
            newState.sAnimInfo.sPath = animPath;
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
