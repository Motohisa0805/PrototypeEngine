#include "HierarchyExplanationPanel.h"
#include "WindowRenderProperty.h"

HierarchyExplanationPanel::HierarchyExplanationPanel(Renderer* renderer)
	:EditorWindow(renderer)
	, mPanelSize(Vector2(WindowRenderProperty::GetWidth() / 2, WindowRenderProperty::GetHeight() / 2))
{
	mID = "HierarchyExplanation";
}

HierarchyExplanationPanel::~HierarchyExplanationPanel()
{
}

void HierarchyExplanationPanel::Initialize(float width, float height, ImTextureRef ref)
{
	EditorWindow::Initialize(width, height, ref);
	mWidthPos = (width / 2) - (mPanelSize.x / 2);
	mHeightPos = (height / 2) - (mPanelSize.y / 2);
	mWidthSize = mPanelSize.x;
	mHeightSize = mPanelSize.y;
	// ウインドウ位置とサイズを固定
	ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
}

void HierarchyExplanationPanel::ResetWindowPos(float width, float height)
{
}

void HierarchyExplanationPanel::Draw(float width, float height)
{
	ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
	EditorWindow::Draw(width, height);
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;
	if (ImGui::Begin(GetImGuiWindowID().c_str(), &mIsShow, flags)) {

		ImGui::SetWindowFontScale(1.25f);
		ImGui::Text("About [Clip Board Operations]");
		ImGui::SetWindowFontScale(1.0f);
		ImGui::Text("You can select an actor and perform various clipboard operations from the menu that appears when you right-click.");
		ImGui::Text("or");
		ImGui::Text("You can operate it from the editor menu on the taskbar.");
		ImGui::Text("or");
		ImGui::Text("You can do it from the shortcut key as well.");

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		ImGui::SetWindowFontScale(1.25f);
		ImGui::Text("About [Add Actor]");
		ImGui::SetWindowFontScale(1.0f);
		ImGui::Text("Right click to display the menu and select [Create Empty Actor]");
		ImGui::Text("or");
		ImGui::Text("Select [Create Empty Actor] within the GameObject on the taskbar");

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();
		
		ImGui::SetWindowFontScale(1.25f);
		ImGui::Text("About [Add Canvas]");
		ImGui::SetWindowFontScale(1.0f);
		ImGui::Text("Right click to display the menu and select [Create Empty Canvas]");
		ImGui::Text("or");
		ImGui::Text("Select [Create Empty Canvas] within the GameObject on the taskbar");
		
		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();
		
		ImGui::SetWindowFontScale(1.25f);
		ImGui::Text("About [Add UIActor]");
		ImGui::SetWindowFontScale(1.0f);
		ImGui::Text("Right click to display the menu and select [Create Empty UIActor]");
		ImGui::Text("or");
		ImGui::Text("Select [Create Empty UIActor] within the GameObject on the taskbar");
	}
	ImGui::End();
}
