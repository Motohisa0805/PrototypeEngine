#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"
#include "SceneSerializer.h"

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
	vector<fs::path>		mDeleteQueue; // 削除予約リスト

	fs::path				mRenameTarget;

	string					mRenameBuffer;
	
	bool					mRenaming;

	bool					mDragDroping;

	bool					mShowOverwritePopup = false;
	// 保留中の操作
	fs::path				mPendingSrc;
	fs::path				mPendingDst;
	// 追加:
	vector<RenameRequest>	mRenameQueue;
	fs::path				mCurrentFolder;
	fs::path				mCurrentFile;
	// ユーザーが左クリックでハイライトしたファイル/フォルダ
	fs::path				mSelectedPath;  
public:
	//選択中のファイルパスを取得
	const char* GetName()override { return "Project"; }
	//コンストラクタ
	ProjectPanel(class Renderer* renderer);
	//GUIの初期化
	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;
	//GUIの描画
	void		Draw(float width, float height, ImTextureRef ref = nullptr)override;
	// Assetsフォルダの右クリックメニュー
	bool        AssetsFolderPrivateMenu();
	// 指定されたディレクトリを再帰的に表示
	void		DrawFolderTree(const fs::path& path);
	void		DrawFileView();
	// 1つのファイル/フォルダを描画
	void		DrawFileSystemEntry(const fs::directory_entry& entry);
	// 右クリックメニュー
	bool		RightClickMenu(const fs::path& path);
	//ショートカットキー入力
	void        ShortcutKeyInputFunction(const fs::path& path);
	//ドラッグ＆ドロップ
	void		DragDropFunction(const fs::path& path);
	//リネーム処理
	void		RenameFunction(const fs::directory_entry entry);
	//上書き確認ポップアップ
	void		DrawOverwritePopup();
	//保留中の削除、リネーム、ドラッグ＆ドロップの処理
	void		ProcessPendingOperations();
};

