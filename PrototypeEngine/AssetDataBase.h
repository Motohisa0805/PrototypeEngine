#pragma once
#include "Typedefs.h"
//サブメッシュ要素
struct SubMeshPayload
{
    char sAssetPath[256];
    char sSubMeshName[256];
    char sLocalID[64];
};
//マテリアル要素
struct MaterialPayload
{
    char sMaterialName[256];
    char sLocalID[64];
};
//アバター要素
struct AvatarPayload
{
    bool sIsAvatar = false;
    filesystem::path sAvatarBinaryPath;
};
//アニメ要素
struct AnimPayload
{
    char             sAnimDataName[256];
    char             sAnimBinaryPath[256];
};

//キャッシュしておくための構造体
struct AssetMetaData
{
    string sGUID;
    vector<SubMeshPayload>  sSubMeshs;  //FBXの場合
    vector<MaterialPayload> sMaterials; //マテリアル情報
    AvatarPayload           sAvatar;    // アバターのキャッシュデータ
    vector<AnimPayload>     sAnims;
};

class AssetDataBase
{
private:
    std::unordered_map<string, AssetMetaData>   mAssetRegistry;
    //GUIDからファイルパスを逆引きするためのマップを追加
    std::unordered_map<string, string>          mGuidToPathMap;

    AssetDataBase();

    AssetDataBase(const AssetDataBase&) = delete;
    AssetDataBase& operator=(const AssetDataBase&) = delete;

public:
    static AssetDataBase& GetInstance()
    {
        static AssetDataBase instance;
        return instance;
    }

    string                  GetAssetPathByGUID(const string& guid);

    AssetMetaData           GetAssetMetaData(const filesystem::path& fbxPath);

    filesystem::path        GeneratedMetaFilePath(const filesystem::path& path);

    vector<SubMeshPayload>  GetSubMeshPayload(const filesystem::path& fbxPath);

    //エンジン起動時やフォルダ更新時に一括で.metaを読み込んでキャッシュを構築
    void                    RefreshDataBase(const std::filesystem::path& assetsDirectory);
    //単一ファイルのキャッシュ更新
    void                    RefreshAssetData(const std::filesystem::path& pastFilePath,const std::filesystem::path& newFilePath);
    //単一ファイルのインポート完了後にキャッシュを更新
    void                    UpdateAssetData(const std::filesystem::path& filePath,const AssetMetaData& data);
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

    bool GetAvatarData(const std::filesystem::path& path, AvatarPayload& outAvatar) const
    {
        auto it = mAssetRegistry.find(path.generic_string());
        if (it != mAssetRegistry.end())
        {
            outAvatar = it->second.sAvatar;
            return true;
        }
        return false;
    }

    bool GetMaterial(const std::filesystem::path& path, vector<MaterialPayload>& outMaterials) const
    {
        auto it = mAssetRegistry.find(path.generic_string());
        if (it != mAssetRegistry.end())
        {
            outMaterials = it->second.sMaterials;
            return true;
        }
        return false;
    }

    bool GetAnimData(const std::filesystem::path& path, std::vector<AnimPayload>& outAnims)const
    {
        auto it = mAssetRegistry.find(path.generic_string());
        if (it != mAssetRegistry.end())
        {
            outAnims = it->second.sAnims;
            return true;
        }
        return false;
    }

};
