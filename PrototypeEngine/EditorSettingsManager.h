#pragma once
#include "Typedefs.h"

//エディタの設定を管理するクラス
//エンジンの状態をJSONファイルに保存し、次回起動時に読み込む
class EditorSettingsManager
{
private:
	static const filesystem::path			SETTEINGS_FILE_PATH;

	//現在のプロジェクト設定を保持するJSONオブジェクト
	json									mSettings;
	
	//シーンが保存されたか
	static bool								mIsNoSaveFlag;

	static string							mRenameInputBuffer;

	static bool								mRenaming;

	static filesystem::path					mCurrentFolder;

	// 削除予約リスト
	static vector<filesystem::path>			mDeleteQueue; 

	EditorSettingsManager() { LoadSettings(); }
	~EditorSettingsManager() { SaveEditorSettings(); }

	//コンストラクタで実行するもの
	void							LoadSettings();
	//シーンを変更した時に保存する情報
	//デストラクタで実行するもの
	void							SaveEditorSettings();
public:
	static EditorSettingsManager&	GetInstance();

	//設定のGetter/Setter
	void							SetLastOpenedScene(const string& path);
	string							GetLastOpenedScene() const;


	//シーン保存フラグのGetter/Setter
	static bool						IsNoSaveFlag() { return mIsNoSaveFlag; }
	static void						SetSaveFlag(bool isSave) { mIsNoSaveFlag = isSave; }

	//リネーム入力バッファのGetter/Setter
	static string					GetRenameInputBuffer() { return mRenameInputBuffer; }
	static void						SetRenameInputBuffer(const string& buffer) { mRenameInputBuffer = buffer; }

	//リネームフラグのGetter/Setter
	static bool						IsRenaming() { return mRenaming; }
	static void						SetRenaming(bool renaming) { mRenaming = renaming; }

	//現在のフォルダのGetter/Setter
	static filesystem::path			GetCurrentFolder() { return mCurrentFolder; }
	static void						SetCurrentFolder(const filesystem::path& folder) { mCurrentFolder = folder; }

	static void						SetDeleteQueue(const filesystem::path& path) { mDeleteQueue.push_back(path); }

	static void						CreateNewScene(const filesystem::path& filePath);

	//ファイル、フォルダの削除は即実行せず、削除予約リストに追加する
	static void 					ProcessPendingDeletions();

	//スクリプトを削除する時の専用の関数
	static 	void 					ProcessScriptDelete(const filesystem::path& path);
};

