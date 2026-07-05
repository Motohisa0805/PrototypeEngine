#pragma once
#include <filesystem>
#include "Typedefs.h"

namespace fs = std::filesystem;

class AssetImporter
{
private:

public:
    // GUIから呼ばれる用の関数。Assetsフォルダ内の全てのFBXファイルをチェックして、必要であればインポートする
	static void CheckAndImportAssets();
    // GUIから呼ばれる用の関数。1つのファイルをチェックして、必要であればインポートする
	static void OneFileCheckAndImportAssets(const filesystem::path& filePath);

	static string GeneratedCustomPath(const string& path);

	static void ConvertFBXToCustomFormat(const string& fbxPath,const string& customPath);
};
