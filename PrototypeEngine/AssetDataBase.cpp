#include "AssetDataBase.h"

AssetDataBase::AssetDataBase() 
    : mAssetRegistry()
{

}

string AssetDataBase::GetAssetPathByGUID(const string& guid)
{
    auto it = mGuidToPathMap.find(guid);
    if (it != mGuidToPathMap.end())
    {
        return it->second;
    }
    return string();
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

    if (!metaJson.contains("cached_data"))
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

                    strncpy_s(info.sAssetPath, sizeof(info.sAssetPath),
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

    
    if (cachedData.contains("materials"))
    {
        vector<MaterialPayload> matPayloads;
        for (const auto& matNode : cachedData["materials"])
        {
            MaterialPayload mat = {};
            string          matName = matNode.value("name", "UnknownMaterial");
            string          localID = matNode.value("local_id", "");

            strncpy_s(mat.sMaterialName,sizeof(mat.sMaterialName),matName.c_str(),_TRUNCATE);
            strncpy_s(mat.sLocalID, sizeof(mat.sLocalID), localID.c_str(),_TRUNCATE);

            matPayloads.push_back(mat);
        }
        data.sMaterials = matPayloads;
    }

    //Avater(SkeletonData)解析
    if (cachedData.contains("skeleton"))
    {
        const auto& skeletonData = cachedData["skeleton"];
        AvatarPayload avatarPayload;
        //アバターがtrueか取得
        if (skeletonData.contains("isAvatar"))
        {
            avatarPayload.sIsAvatar = skeletonData["isAvatar"].get<bool>();
        }
        else
        {
            avatarPayload.sIsAvatar = false;
        }
        if (skeletonData.contains("binary_path"))
        {
            avatarPayload.sAvatarBinaryPath =
                skeletonData["binary_path"].get<string>();
        }
        data.sAvatar = avatarPayload;
    }

    if (cachedData.contains("animations"))
    {
        vector<AnimPayload> animPayloads;
        for (const auto& animNode : cachedData["animations"])
        {
            AnimPayload anim = {};
            
            string animName = animNode.value("clip_name", "");

            strncpy_s(anim.sAnimDataName, sizeof(anim.sAnimDataName), animName.c_str(),_TRUNCATE);

            string animPath = animNode.value("binary_path", "");
            strncpy_s(anim.sAnimBinaryPath, sizeof(anim.sAnimBinaryPath), animPath.c_str(),_TRUNCATE);

            animPayloads.push_back(anim);
        }
        data.sAnims = animPayloads;
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
        if (!entry.is_regular_file())continue;

        auto ext = entry.path().extension();

        if (ext == ".fbx" || ext == ".controller")
        {
            const filesystem::path filePath = entry.path();
            // 対応する独自ファイル
            AssetMetaData data = GetAssetMetaData(filePath);
            UpdateAssetData(filePath, data);
        }
    }
}

void AssetDataBase::RefreshAssetData(const std::filesystem::path& pastFilePath,
                                     const std::filesystem::path& newFilePath)
{
    const string pastKey = pastFilePath.generic_string();
    const string newKey = newFilePath.generic_string();

    auto node = mAssetRegistry.extract(pastKey);
    if (!node.empty())
    {
        node.key() = newKey;
        mAssetRegistry.insert(std::move(node));
    }
    else
    {
        mAssetRegistry[newKey] = GetAssetMetaData(newFilePath);
    }

    const string& guid = mAssetRegistry[newKey].sGUID;
    if (!guid.empty())
    {
        mGuidToPathMap[guid] = newKey;
    }
}

void AssetDataBase::UpdateAssetData(const std::filesystem::path& filePath,
                                    const AssetMetaData&         data)
{
    string pathStr = filePath.generic_string();
    mAssetRegistry[filePath.generic_string()] = data;

    //GUIDが存在していれば逆引きマップにも登録
    if (!data.sGUID.empty())
    {
        mGuidToPathMap[data.sGUID] = pathStr;
    }
}
