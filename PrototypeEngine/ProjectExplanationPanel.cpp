#include "ProjectExplanationPanel.h"
#include "WindowRenderProperty.h"

ProjectExplanationPanel::ProjectExplanationPanel(Renderer* renderer)
    : EditorWindow(renderer)
    , mPanelSize(Vector2(WindowRenderProperty::GetWidth() / 2,
                         WindowRenderProperty::GetHeight() / 2))
{
    mID = "ProjectExplanation";
}

ProjectExplanationPanel::~ProjectExplanationPanel() {}

void ProjectExplanationPanel::Initialize(float width, float height,
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

void ProjectExplanationPanel::ResetWindowPos(float width, float height) {}

void ProjectExplanationPanel::Draw(float width, float height)
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
        ImGui::Text("Two types of operations");
        ImGui::SetWindowFontScale(1.15f);
        ImGui::Text("Right click to display the menu");
        ImGui::Text("or");
        ImGui::Text("Select any menu within the Assets on the taskbar");

        ImGui::Separator();

        ImGui::SetWindowFontScale(1.15f);
        ImGui::Text("About [Create Folder]");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("Add a new folder inside the selected folder");
        ImGui::NewLine();
        ImGui::SetWindowFontScale(1.15f);
        ImGui::Text("[Script(C++)]");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("Add a new script(.h & .cpp) inside the selected folder");
        ImGui::NewLine();
        ImGui::SetWindowFontScale(1.25f);
        ImGui::Text("About [Show in Explorer]");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("Show the selected folder in Explorer");
        ImGui::NewLine();
        ImGui::SetWindowFontScale(1.25f);
        ImGui::Text("About [Open]");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("Open the selected file or folder");
        ImGui::NewLine();
        ImGui::SetWindowFontScale(1.25f);
        ImGui::Text("About [Delete]");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("Delete the selected file or folder");
        ImGui::NewLine();
        ImGui::SetWindowFontScale(1.25f);
        ImGui::Text("About [Rename]");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("Rename the selected file or folder");
        ImGui::NewLine();
        ImGui::SetWindowFontScale(1.25f);
        ImGui::Text("About [Copy]");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("Copy the selected file or folder");

        ImGui::SetWindowFontScale(1.25f);
        ImGui::Text("Drag and drop operation");
        ImGui::SetWindowFontScale(1.15f);
        ImGui::Text(
            "Move the selected file or folder by left-clicking and holding");
    }
    ImGui::End();
}
