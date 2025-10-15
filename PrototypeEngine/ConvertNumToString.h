#pragma once
#include "Typedefs.h"
#include <iostream>
#include <sstream>
#include <iomanip>

/*
* ===エンジン内部処理/Engine internal processing===
*/

namespace FloatToString 
{
	inline string ToStringWithoutDecimal(float value) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << value;
        return oss.str();
    }
};

namespace StringConverter
{
	//拡張子だけを取り除く関数
	inline string RemoveExtension(const std::string& filename) {
		size_t dotPos = filename.rfind('.');
		if (dotPos != std::string::npos) {
			return filename.substr(0, dotPos);  // 拡張子の直前まで切り出す
		}
		return filename;  // ドットがなければそのまま返す
	}
	//指定した文字列を排除する関数
	inline string RemoveString(const std::string& string, const std::string& removeString)
	{
		std::string s;
		//stringからPath部分だけ取り除く
		// removeStringが先頭にあるかチェック
		if (string.find(removeString) == 0) {
			// Model::ModelPathの長さ分だけ取り除いた文字列を取得
			s = string.substr(removeString.length());
		}
		else {
			//SDL_Log("removeString not found at beginning");
			return "";
		}
		return s;
	}
	//指定した範囲の文字列を抽出する関数
	inline string StringExtracter(const string& str, size_t begin, size_t end)
	{
		string result;
		result = str.substr(begin, end);
		return result;
	}

	inline string ExtensionFileName(const string& path_str)
	{
		filesystem::path p(path_str);
		return p.filename().string();
	}

	inline string ExtractFileName_Fs(const string& path_str)
	{
		filesystem::path p(path_str);

		//拡張子を含んだファイル名全体(例：○○〇.json)を取得
		string filename_with_ext = p.filename().string();

		//拡張子部分を取得
		string extension = p.extension().string();

		//拡張子のないファイル名を計算
		if (!extension.empty() && filename_with_ext.size() >= extension.size())
		{
			return filename_with_ext.substr(0, filename_with_ext.length() - extension.length());
		}
		return filename_with_ext;
	}
};

