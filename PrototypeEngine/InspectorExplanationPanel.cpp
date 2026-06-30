#include "InspectorExplanationPanel.h"
#include "WindowRenderProperty.h"

InspectorExplanationPanel::InspectorExplanationPanel(Renderer* renderer)
    : EditorWindow(renderer)
    , mPanelSize(Vector2(WindowRenderProperty::GetWidth() / 2,
                         WindowRenderProperty::GetHeight() / 2))
{
    mID = "InspectorExplanation";
}

InspectorExplanationPanel::~InspectorExplanationPanel() {}

void InspectorExplanationPanel::Initialize(float width, float height,
                                           ImTextureRef ref)
{
    EditorWindow::Initialize(width, height, ref);
    mWidthPos   = (width / 2) - (mPanelSize.x / 2);
    mHeightPos  = (height / 2) - (mPanelSize.y / 2);
    mWidthSize  = mPanelSize.x;
    mHeightSize = mPanelSize.y;
    // ウインドウ位置とサイズを固定
    ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
}

void InspectorExplanationPanel::ResetWindowPos(float width, float height) {}

void InspectorExplanationPanel::Draw(float width, float height)
{
    ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
    EditorWindow::Draw(width, height);
    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoDocking |
                             ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin(GetImGuiWindowID().c_str(), &mIsShow, flags))
    {

        ImGui::SetWindowFontScale(1.25f);
        ImGui::Text("[Game Object Type]");
        ImGui::SetWindowFontScale(1.15f);
        ImGui::Text("Changing a obejct name");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("You can change it in the text box where the object name "
                    "is written.");
        ImGui::NewLine();
        ImGui::SetWindowFontScale(1.15f);
        ImGui::Text("Changing a static flag");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("Please change the flag box to the right of the object "
                    "name text box.");
        ImGui::NewLine();
        ImGui::SetWindowFontScale(1.15f);
        ImGui::Text("Changing a transform");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("Please change the position, rotation, and scale values");
        ImGui::NewLine();
        ImGui::SetWindowFontScale(1.15f);
        ImGui::Text("Add a component");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("Please add it from [Add Component] under the Transform.");
        ImGui::NewLine();
    }
    ImGui::End();
}
