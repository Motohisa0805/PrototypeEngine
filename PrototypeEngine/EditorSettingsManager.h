#pragma once
#include "Typedefs.h"

//エディタの設定を管理するクラス
//エンジンの状態をJSONファイルに保存し、次回起動時に読み込む
class EditorSettingsManager
{
private:
	static const filesystem::path			SETTEINGS_FILE_PATH;

	//現在のプロジェクト設定を保持するJSONオブジェクト
	json							mSettings;
	
	EditorSettingsManager() { LoadSettings(); }
	~EditorSettingsManager() { SaveSettings(); }


	void							LoadSettings();
	void							SaveSettings();
public:
	static EditorSettingsManager&	GetInstance();

	//設定のGetter/Setter
	void							SetLastOpenedScene(const string& path);
	string							GetLastOpenedScene() const;

};

