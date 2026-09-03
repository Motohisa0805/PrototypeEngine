#include "AssetDataBase.h"

AssetDataBase::AssetDataBase() 
    : mAssetRegistry()
{

}

AssetMetaData AssetDataBase::GetAssetMetaData(const filesystem::path& fbxPath)
{
    AssetMetaData  data;
    // FBXパスから対応する.metaファイルのパスを取得
    filesystem::path customPath = GeneratedMetaFilePath(fbxPath);
    //.metaファイルが存在しない場合は空のリストを返す
    if (!filesystem::exists(customPath))
    {
        return data;
    }
    //.metaファイルを読み込む
    std::ifstream inFile(customPath);
    if (!inFile.is_open())
    {
        return data;
    }

    nlohmann::json metaJson;
    inFile >> metaJson;
    inFile.close();

    if (metaJson.contains("guid"))
    {
        data.sGUID = metaJson["guid"].get<string>();
    }

    if (!metaJson.contains("cached_data") ||
        !metaJson["cached_data"].contains("hierarchy"))
    {
        return data;
    }

    const auto& cachedData = metaJson["cached_data"];

    //サブメッシュ解析
    if (cachedData.contains("hierarchy"))
    {
        // スタック(whileループ)
        std::stack<nlohmann::json> nodeStack;
        nodeStack.push(metaJson["cached_data"]["hierarchy"]);
        vector<SubMeshPayload> payloads;
        while (!nodeStack.empty())
        {
            // スタックから1つ取り出す
            auto node = nodeStack.top();
            nodeStack.pop();

            if (node.contains("mesh_indices"))
            {
                string nodeName = node.value("name", "");

                for (const auto& idJson : node["mesh_indices"])
                {
                    string         localID = idJson.get<string>();
                    SubMeshPayload info    = {};

                    strncpy_s(info.sLocalID, sizeof(info.sLocalID),
                              localID.c_str(), _TRUNCATE);

                    strncpy_s(info.sSubMeshName, sizeof(info.sSubMeshName),
                              nodeName.c_str(), _TRUNCATE);

                    payloads.push_back(info);
                }
            }

            if (node.contains("children"))
            {
                for (const auto& child : node["children"])
                {
                    nodeStack.push(child);
                }
            }
        }
        data.sSubMeshs = payloads;
    }

    //Avater(SkeletonData)解析
    if (cachedData.contains("skeleton"))
    {
        const auto& skeletonData = cachedData["skeleton"];
        AvatarPayload avatarPayload;
        avatarPayload.sIsAvatar = true;
        if (skeletonData.contains("binary_path"))
        {
            avatarPayload.sAvatarBinaryPath =
                skeletonData["binary_path"].get<string>();
        }
        data.sAvatar = avatarPayload;
    }
    return data;
}

filesystem::path AssetDataBase::GeneratedMetaFilePath(const filesystem::path& path)
{
    return path.parent_path() / (path.filename().string() + ".meta");
}

vector<SubMeshPayload> AssetDataBase::GetSubMeshPayload(const filesystem::path& fbxPath)
{
    AssetMetaData data = GetAssetMetaData(fbxPath);

    return data.sSubMeshs;
}

void AssetDataBase::RefreshDataBase(
    const std::filesystem::path& assetsDirectory)
{

    for (const auto& entry : filesystem::recursive_directory_iterator(assetsDirectory))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".fbx")
        {
            const filesystem::path fbxPath = entry.path();
            // 対応する独自ファイル
            AssetMetaData data = AssetDataBase::GetInstance().GetAssetMetaData(fbxPath);
            mAssetRegistry[fbxPath.generic_string()]  = data;
        }
    }
}

void AssetDataBase::UpdateAssetData(const std::filesystem::path& filePath,
                                    const AssetMetaData&         data)
{
    mAssetRegistry[filePath.generic_string()] = data;
}
