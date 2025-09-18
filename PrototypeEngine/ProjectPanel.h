#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

namespace fs = std::filesystem;

// 選択中のファイルパスを保持
static std::string g_SelectedFile;

//プロジェクトのファイルパスを描画するクラス
//本格的な描画処理は未実装
class ProjectPanel : public GUIPanel
{
private:
	vector<fs::path> mDeleteQueue; // 削除予約リスト

	fs::path mRenameTarget;

	string mRenameBuffer;
	
	bool mRenaming = false;
public:
	ProjectPanel(class Renderer* renderer);

	void		Draw(float width, float height);
	// 指定されたディレクトリを再帰的に表示
	void		ShowDirectoryRecursive(const fs::path& path);

	bool		RightClickMenu(const fs::path& path);

	void		DeleteFile();

	const char* GetName()override { return "Project"; }
};

