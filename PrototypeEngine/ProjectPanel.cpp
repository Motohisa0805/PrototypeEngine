#include "ProjectPanel.h"

ProjectPanel::ProjectPanel(Renderer* renderer)
	:GUIPanel(renderer)
{
}

void ProjectPanel::Draw(float width, float height)
{
	// ウインドウ位置とサイズを固定
	ImGui::SetNextWindowPos(ImVec2(width * 0.65f, 30));
	ImGui::SetNextWindowSize(ImVec2(width * 0.15f, (float)height - 25));
	//  新しいウィンドウの作成
	ImGui::Begin("Project", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{
        ShowDirectoryRecursive("Assets");
	}
	ImGui::End();
}

void ProjectPanel::ShowDirectoryRecursive(const fs::path& path)
{
	// 指定されたパスがディレクトリでない場合は終了
    // Assetsフォルダ内のファイルを1つずつ処理
    for (auto& entry : fs::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            // フォルダをツリーノードとして表示
            if (ImGui::TreeNode(entry.path().filename().string().c_str()))
            {
                ShowDirectoryRecursive(entry.path()); // 再帰呼び出し
                ImGui::TreePop();
            }
        }
        else
        {
            // ファイル表示
            if (ImGui::Selectable(entry.path().filename().string().c_str()))
            {
                g_SelectedFile = entry.path().string();
            }

            // ファイル右クリックメニュー
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Open in External Editor"))
                {
                    // 例: 外部エディタで開く処理 (OS依存)
                    printf("Open: %s\n", g_SelectedFile.c_str());
                }
                if (ImGui::MenuItem("Delete"))
                {
                    fs::remove(entry.path());
                    printf("Deleted: %s\n", entry.path().string().c_str());
                }
                ImGui::EndPopup();
            }
        }
    }
}
