#include "GUIMainMenu.h"
#include "DebugManager.h"
#include "EditorSettingsManager.h"
#include "EngineWindow.h"
#include "FileOperationManager.h"
#include "GUIEditorManager.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "ProjectPanel.h"
#include "SceneManager.h"
#include "SceneSerializer.h"

GUIMainMenu::GUIMainMenu(Renderer* renderer) : EditorWindow(renderer)
{
    mID = "GUIMainMenu";
}

GUIMainMenu::~GUIMainMenu() {}

void GUIMainMenu::Initialize(float width, float height, ImTextureRef ref)
{
    mWidthPos   = 0.0f;
    mHeightPos  = 0.0f;
    mWidthSize  = width;
    mHeightSize = 25.0f;
    EditorWindow::Initialize(width, height, ref);
    ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
}

void GUIMainMenu::Draw(float width, float height)
{
    ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
    SetPopupColorTheme();
    ImGui::Begin("MainMenu", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    // メインメニューバーの開始
    if (ImGui::BeginMainMenuBar())
    {
        // "File" メニューの開始
        FileMenuDraw();
        // "Editor" メニューの開始
        EditorMenuDraw();
        // "Assets" メニューの開始
        AssetMenuDraw();
        // "GameObject" メニューの開始
        GameObjectMenuDraw();
        //"Component"メニュの開始
        ComponentMenuDraw();
        // "Window" メニューの開始
        WindowMenuDraw();
        //"Help"メニューの開始
        HelpMenuDraw();
        // メインメニューバーの終了
        ImGui::EndMainMenuBar();
    }
    ImGui::End();
    ResetPopupColorTheme();
}

void GUIMainMenu::FileMenuDraw()
{
    // "File" メニューの開始
    if (ImGui::BeginMenu("File"))
    {
        ProjectPanel::CreateNewScene("New Scene", true);

        if (ImGui::MenuItem("Open Scene", "Ctrl + O"))
        {
            FileOperationManager::OpenSceneDialog();
        }

        if (ImGui::MenuItem("Save", "Ctrl+S"))
        {
            string startupScenePath =
                EditorSettingsManager::GetInstance().GetLastOpenedScene();
            SceneSerializer::SaveRunScene(startupScenePath,
                                          SceneManager::GetCurrentRunScene());
            EditorSettingsManager::SetSaveFlag(false);
        }

        if (ImGui::MenuItem("Build Game(Incomplete)"))
        {
            // Build Gameがクリックされた時の処理
            Debug::Log("Build Game clicked!");
        }

        if (ImGui::MenuItem("Exit"))
        {
            EngineWindow::SetEngineState(EngineState::End);
        }

        // "File" メニューの終了
        ImGui::EndMenu();
    }
}

void GUIMainMenu::EditorMenuDraw()
{
    if (ImGui::BeginMenu("Editor"))
    {
        if (ImGui::MenuItem("Undo"))
        {
            CommandManager::Undo();
        }
        if (ImGui::MenuItem("Redo"))
        {
            CommandManager::Redo();
        }
        ImGui::Separator();
        HierarchyPanel::EditorCommandPopupMenu();
        ImGui::EndMenu();
    }
}

void GUIMainMenu::AssetMenuDraw()
{
    if (ImGui::BeginMenu("Assets"))
    {
        if (ImGui::BeginMenu("Create"))
        {
            ProjectPanel::CreateNewFolder();
            ProjectPanel::CreateNewScript();
            ImGui::EndMenu();
        }
        // Show in Explorer（フォルダ・ファイルどちらでも可）
        ProjectPanel::ShowInExplorer();
        if (!filesystem::is_directory(SelectionManager::GetSelectedFilePath()))
        {
            // Open（ファイルのみ）
            ProjectPanel::OpenFile();
        }
        // Delete（フォルダ・ファイルどちらでも可。ただしAssetsフォルダ自体は削除できない）
        ProjectPanel::DeleteFileOrFolder();
        // 名前変更メニュー
        ProjectPanel::RenameMenu();
        // CopyPathメニュー
        ProjectPanel::CopyPathMenu();

        ImGui::EndMenu();
    }
}

void GUIMainMenu::GameObjectMenuDraw()
{
    if (ImGui::BeginMenu("GameObject"))
    {
        if (ImGui::MenuItem("Create New Actor"))
        {
            auto cmd = std::make_unique<CreateNewActorCommand>();
            CommandManager::Execute(std::move(cmd));
            EditorSettingsManager::SetRenameInputBuffer(
                SelectionManager::GetSelectedActor()->GetName());
            EditorSettingsManager::SetRenamingFlag(true);
        }
        if (ImGui::MenuItem("Create New UI Canvas"))
        {
            auto cmd = std::make_unique<CreateNewCanvasCommand>();
            CommandManager::Execute(std::move(cmd));
            EditorSettingsManager::SetRenameInputBuffer(
                SelectionManager::GetSelectedActor()->GetName());
            EditorSettingsManager::SetRenamingFlag(true);
        }
        if (ImGui::MenuItem("Create New UI Actor"))
        {
            auto cmd = std::make_unique<CreateNewUIActorCommand>();
            CommandManager::Execute(std::move(cmd));
            EditorSettingsManager::SetRenameInputBuffer(
                SelectionManager::GetSelectedActor()->GetName());
            EditorSettingsManager::SetRenamingFlag(true);
        }
        ImGui::EndMenu();
    }
}

void GUIMainMenu::ComponentMenuDraw()
{
    if (ImGui::BeginMenu("Component"))
    {
        ImGui::BeginDisabled(!SelectionManager::GetSelectedActor());
        InspectorPanel::ComponentSelectorDraw(
            SelectionManager::GetSelectedActor());
        ImGui::EndDisabled();
        ImGui::EndMenu();
    }
}

void GUIMainMenu::WindowMenuDraw()
{
    if (ImGui::BeginMenu("Window"))
    {
        if (ImGui::BeginMenu("Layout"))
        {
            if (ImGui::BeginMenu("Show"))
            {
                if (ImGui::MenuItem("GameViewEditor"))
                {
                    GUIEditorManager::GetRootMainWindow()->AddEditorWindow(
                        EditorWindowFactory::CreateEditorWindow("GameView",
                                                                mRenderer));
                }
                if (ImGui::MenuItem("SceneViewEditor"))
                {
                    GUIEditorManager::GetRootMainWindow()->AddEditorWindow(
                        EditorWindowFactory::CreateEditorWindow("SceneView",
                                                                mRenderer));
                }
                if (ImGui::MenuItem("HierarchyEditor"))
                {
                    GUIEditorManager::GetRootMainWindow()->AddEditorWindow(
                        EditorWindowFactory::CreateEditorWindow("Hierarchy",
                                                                mRenderer));
                }
                if (ImGui::MenuItem("ProjectEditor"))
                {
                    GUIEditorManager::GetRootMainWindow()->AddEditorWindow(
                        EditorWindowFactory::CreateEditorWindow("Project",
                                                                mRenderer));
                }
                if (ImGui::MenuItem("InspectorEditor"))
                {
                    GUIEditorManager::GetRootMainWindow()->AddEditorWindow(
                        EditorWindowFactory::CreateEditorWindow("Inspector",
                                                                mRenderer));
                }
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Save Layout"))
            {
                GUIEditorManager::SaveCurrentLayout("Library/myLayout.ini");
            }

            if (ImGui::MenuItem("Load Layout"))
            {
                GUIEditorManager::LoadCustomLayout("Library/myLayout.ini");
            }
            ImGui::EndMenu();
        }

        // TODO : 今後追加予定処理
        /*
        if (ImGui::MenuItem("2 by 3(2 * 3) Layuot"))
        {
            GUIEditorManager::ApplyDefaultLayout_2by3();
        }
        */

        if (ImGui::BeginMenu("Rendering"))
        {

            if (ImGui::MenuItem("Lighting"))
            {
                GUIEditorManager::GetRootMainWindow()->AddEditorWindow(
                    EditorWindowFactory::CreateEditorWindow("LightingPanel",
                                                            mRenderer));
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
}

void GUIMainMenu::HelpMenuDraw()
{
    if (ImGui::BeginMenu("Help"))
    {
        if (ImGui::MenuItem("About PrototypeEngine"))
        {
            GUIEditorManager::GetRootMainWindow()->AddEditorWindow(
                EditorWindowFactory::CreateEditorWindow("AboutEngine",
                                                        mRenderer));
        }

        if (ImGui::BeginMenu("Instructions"))
        {
            if (ImGui::MenuItem("HierarchyExplanation"))
            {
                GUIEditorManager::GetRootMainWindow()->AddEditorWindow(
                    EditorWindowFactory::CreateEditorWindow(
                        "HierarchyExplanation", mRenderer));
            }
            if (ImGui::MenuItem("ProjectExplanation"))
            {
                GUIEditorManager::GetRootMainWindow()->AddEditorWindow(
                    EditorWindowFactory::CreateEditorWindow(
                        "ProjectExplanation", mRenderer));
            }
            if (ImGui::MenuItem("InspectorExplanation"))
            {
                GUIEditorManager::GetRootMainWindow()->AddEditorWindow(
                    EditorWindowFactory::CreateEditorWindow(
                        "InspectorExplanation", mRenderer));
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

void GUIMainMenu::SetPopupColorTheme()
{
    EditorWindow::SetPopupColorTheme();
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.9f, 0.9f, 0.9f, 0.9f));
}

void GUIMainMenu::ResetPopupColorTheme() { ImGui::PopStyleColor(4); }
