#pragma once
#include "Typedefs.h"

//エディタの設定を管理するクラス
//エンジンの状態をJSONファイルに保存し、次回起動時に読み込む
class EditorSettingsManager
{
private:
	static const filesystem::path			SETTEINGS_FILE_PATH;

	//現在のプロジェクト設定を保持するJSONオブジェクト
	json									mEditorSettingData;
	
	//シーンが保存されたか
	static bool								mIsNoSaveFlag;

	static string							mRenameInputBuffer;

	static bool								mIsRenaming;

	// 削除予約リスト
	static vector<filesystem::path>			mDeleteDirectoryQueue; 

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
	static bool						IsRenaming() { return mIsRenaming; }
	static void						SetRenamingFlag(bool renaming) { mIsRenaming = renaming; }

	static void						SetDeleteDirectoryQueue(const filesystem::path& path) { mDeleteDirectoryQueue.push_back(path); }

	static void						CreateNewScene(const filesystem::path& filePath, const filesystem::path& selectingPath);

	//ファイル、フォルダの削除は即実行せず、削除予約リストに追加する
	static void 					ProcessPendingDeletions();

	//スクリプトを削除する時の専用の関数
	static 	void 					ProcessScriptDelete(const filesystem::path& path);
};