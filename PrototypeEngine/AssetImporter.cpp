#include "AssetImporter.h"

void AssetImporter::CheckAndImportAssets()
{ 
	string assetsDir = "Assets/";

	for (const auto& entry : fs::recursive_directory_iterator(assetsDir)) {
		if (entry.is_regular_file() && entry.path().extension() == ".fbx") {
            string fbxPath = entry.path().string();
			//対応する独自ファイル
            string customPath = GeneratedCustomPath(fbxPath);
			//独自ファイルが存在しない、またはFBXファイルの方が新しく更新されている場合
			if (!fs::exists(customPath) || fs::last_write_time(entry) > fs::last_write_time(customPath)){
				//ここでFBXを読み込み、独自ファイルへ書き出す処理を呼ぶ
                ConvertFBXToCustomFormat(fbxPath, customPath);
            }
		}
	}
}

void AssetImporter::OneFileCheckAndImportAssets(
    const filesystem::path& filePath)
{
    string assetsDir = "Assets/";

    for (const auto& entry : fs::recursive_directory_iterator(assetsDir))
    {
        if (entry.is_regular_file() && entry.path() == filePath)
        {
            string fbxPath = entry.path().string();
            // 対応する独自ファイル
            string customPath = GeneratedCustomPath(fbxPath);
            // 独自ファイルが存在しない、またはFBXファイルの方が新しく更新されている場合
            if (!fs::exists(customPath) ||
                fs::last_write_time(entry) > fs::last_write_time(customPath))
            {
                // ここでFBXを読み込み、独自ファイルへ書き出す処理を呼ぶ
                ConvertFBXToCustomFormat(fbxPath, customPath);
            }
        }
    }
}

string AssetImporter::GeneratedCustomPath(const string& path)
{
    return string();
}

void AssetImporter::ConvertFBXToCustomFormat(const string& fbxPath,const string& customPath)
{
}
