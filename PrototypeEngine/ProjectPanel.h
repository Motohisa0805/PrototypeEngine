#pragma once
#include "EditorWindow.h"

//プロジェクトのファイルパスを描画するクラス
//本格的な描画処理は未実装
class ProjectPanel : public EditorWindow
{
private:
	//名前変更関係の変数
	static filesystem::path			mPathToRename;
	//名前変更の入力バッファ
	static string					mRenameInputBuffer;
	//名前変更モードかどうか
	static bool						mIsRenaming;


	bool							mShowOverwritePopup = false;
	// 保留中の操作
	filesystem::path				mPendingSrc;
	filesystem::path				mPendingDst;

	// ユーザーが左クリックでハイライトしたファイル/フォルダ

	static filesystem::path			mSelectedFolderPath;


	static char						mScriptCreateBuffer[256];

	static bool						mIsShowScriptPopup;
public:

	static void 					SetRenameInputBuffer(const string& input) { mRenameInputBuffer = input; }
	
	static void 					SetRenaming(bool renaming) { mIsRenaming = renaming; }

	static filesystem::path			GetSelectedFolderPath() { return mSelectedFolderPath; }
	//コンストラクタ
									ProjectPanel(Renderer* renderer);
	//GUIの初期化
	void							Initialize(float width, float height, ImTextureRef ref = nullptr)override;
	//GUIの描画
	void							Draw(float width, float height)override;
	// 指定されたディレクトリを再帰的に表示
	void							DrawFolderTree(const filesystem::path& path);
	void							DrawPickUpFolderView();
	// 1つのファイルを描画
	void							DrawFileSystemEntry(const filesystem::directory_entry& entry);
	//タブのドッキング設定時に呼び出す
	static void						SetDockWindow(ImGuiID id, ImGuiID& outID);

	// 右クリックメニュー
	bool							RightClickMenu();
	static void						CreateNewFolder();
	static void						CreateNewScene(const string& name,bool loadScene = false);
	static void						CreateNewScript();
	static void						ShowInExplorer();
	static void						OpenFile();
	static void						DeleteFileOrFolder();
	//名前変更メニューのカプセル化
	static void						RenameMenu();
	//CopyPathメニューのカプセル化
	static void						CopyPathMenu();

	//ショートカットキー入力
	void							ShortcutKeyInputFunction(const filesystem::path& path);
	//ドラッグ＆ドロップ
	void							DragDropFunction(const filesystem::path& path);
	//リネーム処理
	void							RenameFunction(const filesystem::directory_entry entry);
	//上書き確認ポップアップ
	void							DrawOverwritePopup();

	void							DrawScriptCreatePopup();
};