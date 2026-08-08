#pragma once
#include "Typedefs.h"

struct SubMeshPayload
{
    char sSubMeshName[256];
    char sLocalID[64];
};

//キャッシュしておくための構造体
struct AssetMetaData
{
    string sGUID;
    vector<SubMeshPayload> sSubMeshs;//FBXの場合
    //TODO : ファイルのキャッシュデータ追加は今後ここに予定
};

class AssetDataBase
{
private:
    std::unordered_map<string, AssetMetaData> mAssetRegistry;

    AssetDataBase();

    AssetDataBase(const AssetDataBase&) = delete;
    AssetDataBase& operator=(const AssetDataBase&) = delete;

public:
    static AssetDataBase& GetInstance()
    {
        static AssetDataBase instance;
        return instance;
    }

    AssetMetaData GetAssetMetaData(const filesystem::path& fbxPath);

    filesystem::path GeneratedMetaFilePath(const filesystem::path& path);

    vector<SubMeshPayload> GetSubMeshPayload(const filesystem::path& fbxPath);

    //エンジン起動時やフォルダ更新時に一括で.metaを読み込んでキャッシュを構築
    void RefreshDataBase(const std::filesystem::path& assetsDirectory);
    //単一ファイルのインポート完了後にキャッシュを更新
    void UpdateAssetData(const std::filesystem::path& filePath,const AssetMetaData& data);
    //プロジェクトパネルからO(1)で高速に情報を取得する
    bool GetSubMeshs(const std::filesystem::path& path, std::vector<SubMeshPayload>& outSubMeshs)const
    {
        auto it = mAssetRegistry.find(path.generic_string());
        if (it != mAssetRegistry.end())
        {
            outSubMeshs = it->second.sSubMeshs;
            return true;
        }
        return false;
    }

};
