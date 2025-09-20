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
public:
	struct RenameRequest 
	{
		fs::path oldPath;
		std::string newStem; // 拡張子は oldPath.extension() を使って再結合する
	};
private:
	vector<fs::path> mDeleteQueue; // 削除予約リスト

	fs::path mRenameTarget;

	string mRenameBuffer;
	
	bool mRenaming;

	bool mDragDroping;

	bool mShowOverwritePopup = false;
	fs::path mPendingSrc;
	fs::path mPendingDst;

	// 追加:
	vector<RenameRequest> mRenameQueue;

	fs::path mCurrentFolder;
public:
	ProjectPanel(class Renderer* renderer);

	void		Draw(float width, float height);

	// 指定されたディレクトリを再帰的に表示
	void		DrawFolderTree(const fs::path& path);
	void		DrawFileView();

	bool        AssetsFolderPrivateMenu();

	bool		RightClickMenu(const fs::path& path);

	void		DragDropFunction(const fs::path& path);

	void		DrawOverwritePopup();

	void		RenameFunction(const fs::directory_entry entry);

	void		ProcessPendingOperations();

	const char* GetName()override { return "Project"; }
};

