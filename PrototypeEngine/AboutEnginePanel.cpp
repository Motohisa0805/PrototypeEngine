#include "AboutEnginePanel.h"
#include "WindowRenderProperty.h"

AboutEnginePanel::AboutEnginePanel(Renderer* renderer)
    : EditorWindow(renderer)
    , mPanelSize(Vector2(WindowRenderProperty::GetWidth() / 2,
                         WindowRenderProperty::GetHeight() / 2))
{
    mID = "AboutEngine";
}

AboutEnginePanel::~AboutEnginePanel() {}

void AboutEnginePanel::Initialize(float width, float height, ImTextureRef ref)
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

void AboutEnginePanel::ResetWindowPos(float width, float height) {}

void AboutEnginePanel::Draw(float width, float height)
{
    ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
    EditorWindow::Draw(width, height);
    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoDocking |
                             ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin(GetImGuiWindowID().c_str(), &mIsShow, flags))
    {

        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("PrototypeEngine");
        ImGui::SetWindowFontScale(1.0f);

        ImGui::Text("Version 0.01");
        ImGui::Text("Developer: [Kusume Motohisa]");
        ImGui::Text("Project GitHub URL");
        ImGui::TextLinkOpenURL(
            "GitHub URL", "https://github.com/Motohisa0805/PrototypeEngine");

        ImGui::Separator();

        // 3. 使用しているサードパーティ製ライブラリのクレジット
        ImGui::TextDisabled("Powered by Great Open Source Libraries:");
        ImGui::Spacing();

        if (ImGui::TreeNode("Dear ImGui"))
        {
            ImGui::TextWrapped("Copyright (c) 2014-2026 Omar Cornut\nLicensed "
                               "under the MIT License.");
            ImGui::TextLinkOpenURL("Dear ImGui GitHub URL",
                                   "https://github.com/ocornut/imgui");
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("SDL3 (Simple DirectMedia Layer)"))
        {
            ImVec2 size = ImVec2((float)EditorTextureManager::GetInstance()
                                     .GetSDL3Logo()
                                     ->GetWidth(),
                                 (float)EditorTextureManager::GetInstance()
                                     .GetSDL3Logo()
                                     ->GetHeight());
            ImGui::Image(
                (ImTextureID)(intptr_t)EditorTextureManager::GetInstance()
                    .GetSDL3Logo()
                    ->GetTextureID(),
                ImVec2(size.x, size.y));
            ImGui::TextWrapped("Copyright (C) 1997-2026 Sam Lantinga\nLicensed "
                               "under the Zlib License.");
            ImGui::TextLinkOpenURL("SDL3 Official Page URL",
                                   "https://wiki.libsdl.org/SDL3/FrontPage");
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("<OpenGL / GLEW>2.2.0"))
        {
            ImVec2 size = ImVec2((float)EditorTextureManager::GetInstance()
                                     .GetOpenGLLogo()
                                     ->GetWidth(),
                                 (float)EditorTextureManager::GetInstance()
                                     .GetOpenGLLogo()
                                     ->GetHeight());
            ImGui::Image(
                (ImTextureID)(intptr_t)EditorTextureManager::GetInstance()
                    .GetOpenGLLogo()
                    ->GetTextureID(),
                ImVec2(size.x, size.y));
            ImGui::TextWrapped("Graphics API & Extension Wrangler Library.");
            ImGui::TextLinkOpenURL("OpenGL Official Page URL",
                                   "https://www.opengl.org/");
            ImGui::TreePop();
        }

        ImGui::Separator();
    }
    ImGui::End();
}
