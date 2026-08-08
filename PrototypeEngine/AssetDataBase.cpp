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

    if (!metaJson.contains("cached_data") ||
        !metaJson["cached_data"].contains("hierarchy"))
    {
        return data;
    }

    if (metaJson.contains("guid"))
    {
        data.sGUID = metaJson["guid"].get<string>();
    }

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

                strncpy_s(info.sLocalID, sizeof(info.sLocalID), localID.c_str(),
                          _TRUNCATE);

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
    return data;
}

filesystem::path AssetDataBase::GeneratedMetaFilePath(const filesystem::path& path)
{
    return path.parent_path() / (path.filename().string() + ".meta");
}

vector<SubMeshPayload> AssetDataBase::GetSubMeshPayload(const filesystem::path& fbxPath)
{
    vector<SubMeshPayload> payloads;
    // FBXパスから対応する.metaファイルのパスを取得
    filesystem::path customPath =
        AssetDataBase::GetInstance().GeneratedMetaFilePath(fbxPath);
    //.metaファイルが存在しない場合は空のリストを返す
    if (!filesystem::exists(customPath))
    {
        return payloads;
    }
    //.metaファイルを読み込む
    std::ifstream inFile(customPath);
    if (!inFile.is_open())
    {
        return payloads;
    }

    nlohmann::json metaJson;
    inFile >> metaJson;
    inFile.close();

    if (!metaJson.contains("cached_data") ||
        !metaJson["cached_data"].contains("hierarchy"))
    {
        return payloads;
    }

    // スタック(whileループ)
    std::stack<nlohmann::json> nodeStack;
    nodeStack.push(metaJson["cached_data"]["hierarchy"]);

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

                strncpy_s(info.sLocalID, sizeof(info.sLocalID), localID.c_str(),
                          _TRUNCATE);

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

    return payloads;
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
