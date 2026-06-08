#include "GUIMainMenu.h"
#include "EditorSettingsManager.h"
#include "SceneSerializer.h"
#include "SceneManager.h"
#include "DebugManager.h"
#include "GUIEditorManager.h"
#include "EngineWindow.h"
#include "BaseScene.h"
#include "ProjectPanel.h"
#include "HierarchyPanel.h"

GUIMainMenu::GUIMainMenu(Renderer* renderer)
	:EditorWindow(renderer)
{
	mID = "GUIMainMenu";
}

GUIMainMenu::~GUIMainMenu()
{
}

void GUIMainMenu::Initialize(float width, float height, ImTextureRef ref)
{
	mWidthPos = 0.0f;
	mHeightPos = 0.0f;
	mWidthSize = width;
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
	ImGui::Begin("MainMenu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
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
		ProjectPanel::CreateNewScene("New Scene");

		if (ImGui::MenuItem("Open Scene", "Ctrl + O")) {
			FileOperationManager::OpenSceneDialog();
		}

		if (ImGui::MenuItem("Save","Ctrl+S"))
		{
			string startupScenePath = EditorSettingsManager::GetInstance().GetLastOpenedScene();
			SceneSerializer::SaveScene(startupScenePath, SceneManager::GetNowScene());
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
		if (!filesystem::is_directory(ProjectPanel::GetSelectedPath()))
		{
			// Open（ファイルのみ）
			ProjectPanel::OpenFile();
		}
		// Delete（フォルダ・ファイルどちらでも可。ただしAssetsフォルダ自体は削除できない）
		ProjectPanel::DeleteFileOrFolder();
		//名前変更メニュー
		ProjectPanel::RenameMenu();
		//CopyPathメニュー
		ProjectPanel::CopyPathMenu();

		ImGui::EndMenu();
	}
}

void GUIMainMenu::GameObjectMenuDraw()
{
	if (ImGui::BeginMenu("GameObject"))
	{
		if (ImGui::MenuItem("Create 3D Empty"))
		{
			auto cmd = std::make_unique<CreateNewActorCommand>();
			CommandManager::Execute(std::move(cmd));
			EditorSettingsManager::SetRenameInputBuffer(SelectionManager::GetSelectedActor()->GetName());
			EditorSettingsManager::SetRenaming(true);
		}
		if (ImGui::MenuItem("Create Empty Canvas")) {
			auto cmd = std::make_unique<CreateNewCanvasCommand>();
			CommandManager::Execute(std::move(cmd));
			EditorSettingsManager::SetRenameInputBuffer(SelectionManager::GetSelectedActor()->GetName());
			EditorSettingsManager::SetRenaming(true);
		}
		if (ImGui::MenuItem("Create 2D Empty"))
		{
			auto cmd = std::make_unique<CreateNewUIActorCommand>();
			CommandManager::Execute(std::move(cmd));
			EditorSettingsManager::SetRenameInputBuffer(SelectionManager::GetSelectedActor()->GetName());
			EditorSettingsManager::SetRenaming(true);
		}
		ImGui::EndMenu();
	}
}

void GUIMainMenu::ComponentMenuDraw()
{
	if (ImGui::BeginMenu("Component"))
	{
		if (ImGui::MenuItem("Incomplete"))
		{

		}
		ImGui::EndMenu();
	}
}

void GUIMainMenu::WindowMenuDraw()
{
	if (ImGui::BeginMenu("Window"))
	{
		if (ImGui::BeginMenu("Show")) {
			if (ImGui::MenuItem("GameViewEditor")) {
				GUIEditorManager::GetRootMainWindow()->AddEditorWindow(EditorWindowFactory::CreateEditorWindow("GameView"));
			}
			if (ImGui::MenuItem("SceneViewEditor")) {
				GUIEditorManager::GetRootMainWindow()->AddEditorWindow(EditorWindowFactory::CreateEditorWindow("SceneView"));
			}
			if (ImGui::MenuItem("HierarchyEditor")) {
				GUIEditorManager::GetRootMainWindow()->AddEditorWindow(EditorWindowFactory::CreateEditorWindow("Hierarchy"));
			}
			if (ImGui::MenuItem("ProjectEditor")) {
				GUIEditorManager::GetRootMainWindow()->AddEditorWindow(EditorWindowFactory::CreateEditorWindow("Project"));
			}
			if (ImGui::MenuItem("InspectorEditor")) {
				GUIEditorManager::GetRootMainWindow()->AddEditorWindow(EditorWindowFactory::CreateEditorWindow("Inspector"));
			}
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("2 by 3(2×3) Layuot(Incomplete)"))
		{

		}
		ImGui::EndMenu();
	}
}

void GUIMainMenu::HelpMenuDraw()
{
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("About PrototypeEngine")){

		}

		if (ImGui::MenuItem("Tutorial")) {

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

void GUIMainMenu::ResetPopupColorTheme()
{
	ImGui::PopStyleColor(4);
}
