#include "LightingPanel.h"
#include "EngineWindow.h"

LightingPanel::LightingPanel(Renderer* renderer)
	:EditorWindow(renderer)
{
	mID = "LightingPanel";
}

LightingPanel::~LightingPanel()
{
}

void LightingPanel::Initialize(float width, float height, ImTextureRef ref)
{
	EditorWindow::Initialize(width, height, ref);
}

void LightingPanel::Draw(float width, float height)
{
	EditorWindow::Draw(width, height);
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;
	if (ImGui::Begin(GetImGuiWindowID().c_str(), &mIsShow, flags)) {
		//1.ファイルパスの取得
		string currentPath = EngineWindow::GetRenderer()->GetSkyBoxRenderer()->GetLoadFilePath();
		static char pathBuffer[256];
		strncpy_s(pathBuffer, currentPath.c_str(), sizeof(pathBuffer));
		pathBuffer[sizeof(pathBuffer) - 1] = '\0';
		ImGui::Text("FilePath DragDropTarget");
		//2.ファイルパスの入力フィールド
		ImGui::InputText("SkyBoxTexture File Path", pathBuffer, sizeof(pathBuffer), ImGuiInputTextFlags_ReadOnly);
		//3.ファイルロードボタン(ここでファイル選択UIを開くか、ProjectPanelからのDrag&Dropを想定)
		//Drag&Drop想定
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				//ペイロードがファイルパスであると仮定
				const char* dropPath = (const char*)payload->Data;
				if (EngineWindow::GetRenderer()->GetSkyBoxRenderer()->Load(dropPath)) {
					EngineWindow::GetRenderer()->GetRunScene()->SetLoadSkyBoxTexturePath(dropPath);
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
	ImGui::End();
}
