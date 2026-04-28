#pragma once
#include "Typedefs.h"

class ScriptEditManager
{
private:
	ScriptEditManager();
	//実行ファイルから .vcxprojファイルへの相対パス
	filesystem::path mVcxppoj_Path;
public:
	static ScriptEditManager& GetInstance()
	{
		//静的変数としてインスタンスを定義
		static ScriptEditManager instance;
		return instance;
	}
	//コピーコンストラクタと代入演算子を削除
	ScriptEditManager(const ScriptEditManager&) = delete;
	ScriptEditManager& operator = (const ScriptEditManager&) = delete;


	filesystem::path GetVcxppoj_Path() { return mVcxppoj_Path; }

	//スクリプトファイルを生成
	//例：folderPath = "Assets/Scripts/" scriptName = "MyScript"
	bool CreateScriptFile(const std::filesystem::path& folderPath, const std::string& scriptName);
	//生成されたスクリプトファイルの書き戻す関数
	bool ReplaceInFile(const filesystem::path& filePath, const string& oldStr, const string& newStr);

	bool DoesEntryExist(tinyxml2::XMLElement* itemGroup, const char* tagName, const char* includePath);

	bool AddScriptFileToVcxProj(const filesystem::path& path, const string& scriptClassName);
	bool RemoveScriptFileToVcxProj(const filesystem::path& path,const string& scriptClassName);
};

