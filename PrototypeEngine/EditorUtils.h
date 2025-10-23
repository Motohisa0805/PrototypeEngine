#pragma once
#include "Typedefs.h"
#include "DebugManager.h"
#include "ConvertNumToString.h"

class EditorUtils
{
private:
	EditorUtils();

public:
	static EditorUtils& GetInstance()
	{
		//静的変数としてインスタンスを定義
		static EditorUtils instance;
		return instance;
	}
	//コピーコンストラクタと代入演算子を削除
	EditorUtils(const EditorUtils&) = delete;
	EditorUtils& operator = (const EditorUtils&) = delete;

	//スクリプトファイルを生成
	bool CreateScriptFile(const std::filesystem::path& folderPath, const std::string& scriptName);
	//生成されたスクリプトファイルの書き戻す関数
	bool ReplaceInFile(const filesystem::path& filePath, const string& oldStr, const string& newStr);

	bool AddScriptFileToVcxProj(const string& scriptClassName);
	bool RemoveScriptFileToVcxProj(const string& scriptClassName);
};

