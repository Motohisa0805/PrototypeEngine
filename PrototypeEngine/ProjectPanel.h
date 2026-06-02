#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//プロジェクトのファイルパスを描画するクラス
//本格的な描画処理は未実装
class ProjectPanel : public GUIPanel
{
private:

	filesystem::path				mPathToRename;

	string							mRenameInputBuffer;
	
	bool							mRenaming;

	bool							mShowOverwritePopup = false;
	// 保留中の操作
	filesystem::path				mPendingSrc;
	filesystem::path				mPendingDst;

	vector<RenameRequest>			mRenameQueue;

	filesystem::path				mCurrentFile;
	// ユーザーが左クリックでハイライトしたファイル/フォルダ
	static filesystem::path			mSelectedPath;

	static filesystem::path			mScriptFilePath;
public:
	//選択中のファイルパスを取得
	const char* GetName()override { return "Project"; }
	
	static filesystem::path			GetSelectedPath() { return mSelectedPath; }
	//コンストラクタ
	ProjectPanel(class Renderer* renderer);
	//GUIの初期化
	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;
	//GUIの描画
	void		Draw(float width, float height, ImTextureRef ref = nullptr)override;
	// 指定されたディレクトリを再帰的に表示
	void		DrawFolderTree(const filesystem::path& path);
	void		DrawPickUpFolderView();
	// 1つのファイル/フォルダを描画
	void		DrawFileSystemEntry(const filesystem::directory_entry& entry);
	// 右クリックメニュー
	bool		RightClickMenu();
	//ショートカットキー入力
	void        ShortcutKeyInputFunction(const filesystem::path& path);
	//ドラッグ＆ドロップ
	void		DragDropFunction(const filesystem::path& path);
	//リネーム処理
	void		RenameFunction(const filesystem::directory_entry entry);
	//上書き確認ポップアップ
	void		DrawOverwritePopup();
	//保留中の削除、リネーム、ドラッグ＆ドロップの処理
	void		ProcessPendingOperations();
};

