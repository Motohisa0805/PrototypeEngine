#include "AssetImporter.h"

void AssetImporter::CheckAndImportAssets()
{ 
	string assetsDir = "Assets/";

	for (const auto& entry : fs::recursive_directory_iterator(assetsDir)) {
		if (entry.is_regular_file() && entry.path().extension() == ".fbx") {
            fs::path fbxPath = entry.path();
			//対応する独自ファイル
            fs::path customPath = GeneratedCustomPath(fbxPath);
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
            fs::path fbxPath = entry.path();
            // 対応する独自ファイル
            fs::path customPath = GeneratedCustomPath(fbxPath);
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

string AssetImporter::GenerateUUID()
{
    UUID uuid;
    // バイナリ形式のUUIDを生成
    if (UuidCreate(&uuid) != RPC_S_OK)
    {
        return ""; // UUID生成に失敗した場合は空文字を返す
    }

    RPC_CSTR uuidStr = nullptr;
    if (UuidToStringA(&uuid, &uuidStr) != RPC_S_OK)
    {
        return "";
    }
    
    string result(reinterpret_cast<char*>(uuidStr));

    // メモリを解放
    RpcStringFreeA(&uuidStr);

    return result; 
}

fs::path AssetImporter::GeneratedCustomPath(const fs::path& path)
{
    return path.parent_path() / (path.filename().string() + ".meta");
}

void AssetImporter::ConvertFBXToCustomFormat(const fs::path& fbxPath,
                                             const fs::path& customPath)
{
    nlohmann::json metaJson;
    metaJson["fileFormatVersion"] = 1;
    metaJson["guid"] = GenerateUUID();

    metaJson["import_mesh"] = true;
    metaJson["import_skeleton"] = true;
    metaJson["import_animation"] = true;

    metaJson["model_settings"] = {
        {"scale_factor", 1.0},
    };

    metaJson["skeleton_settings"] = {
        {"skeleton_type", 0},
    };

    metaJson["animation_settings"] = {
        {"is_looping", true},
        {"start_frame", 0},
        {"end_frame", -1},
    };

    // JSONをファイルに書き込む
    std::ofstream outFile(customPath);
    if (outFile.is_open())
    {
        outFile << metaJson.dump(4); // インデント幅を4に設定して整形
        outFile.close();
    }
    else
    {
        std::cerr << "Failed to open file for writing: " << customPath << std::endl;
    }
}
