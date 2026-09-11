#include "MaterialGenerater.h"
#include "DebugManager.h"

fs::path MaterialGenerater::GeneratedMatFilePath(const string& fileName)
{
    return fileName + ".mat";
}

bool MaterialGenerater::GeneratedBlankMaterial(const fs::path& path) 
{
    try
    {
        nlohmann::json matJson;

        //メタ情報
        matJson["version"] = 1;
        matJson["shader"]  = "StandardPBR";

        //基本カラー・パラメーター
        matJson["properties"]["diffuse_color"] = {1.0f, 1.0f, 1.0f, 1.0f};
        matJson["properties"]["ambient_color"] = {0.2f, 0.2f, 0.2f};
        matJson["properties"]["shininess"] = 0.390625f;

        matJson["properties"]["metallic"] = 0.0f;
        matJson["properties"]["roughness"] = 0.0f;
        matJson["properties"]["emissive"]  = {0.0f, 0.0f, 0.0f};

        //テクスチャスロット(初期状態は空文字)
        matJson["textures"]["albedo_map"] = "";
        matJson["textures"]["normal_map"] = "";
        matJson["textures"]["specular_map"] = "";

        //ファイルの書き出し
        std::ofstream outFile(path);
        if (!outFile.is_open())
        {
            Debug::Log("Failed to open file for writing: %s\n", path.string().c_str());
            return false;
        }

        outFile << matJson.dump(4);
        outFile.close();
        return true;
    }
    catch (const std::exception& e)
    {
        Debug::Log("Exception in GeneratedBlankMaterial: %s\n", e.what());
        return false;
    }
}

bool MaterialGenerater::GenerateMaterial(const fs::path&           path,
                                         const MaterialParameters& params)
{
    try
    {
        nlohmann::json matJson;

        // メタ情報
        matJson["version"] = 1;
        matJson["shader"]  = params.sShader;

        // 基本カラー・パラメーター
        matJson["properties"]["diffuse_color"] = {params.sDiffuse_color[0], params.sDiffuse_color[1],
                                                  params.sDiffuse_color[2], params.sDiffuse_color[3]};
        matJson["properties"]["ambient_color"] = {params.sAmbinet_color[0],params.sAmbinet_color[1],params.sAmbinet_color[2]};
        matJson["properties"]["shininess"]     = params.sShininess;

        matJson["properties"]["metallic"]  = params.sMetallic;
        matJson["properties"]["roughness"] = params.sRoughness;
        matJson["properties"]["emissive"]  = {params.sEmissive[0], params.sEmissive[1],params.sEmissive[2]};

        // テクスチャスロット(初期状態は空文字)
        matJson["textures"]["albedo_map"]   = params.sAlbedo_map;
        matJson["textures"]["normal_map"]   = params.sNormal_map;
        matJson["textures"]["specular_map"] = params.sSpecular_map;

        // ファイルの書き出し
        std::ofstream outFile(path);
        if (!outFile.is_open())
        {
            Debug::Log("Failed to open file for writing: %s\n",
                       path.string().c_str());
            return false;
        }

        outFile << matJson.dump(4);
        outFile.close();
        return true;
    }
    catch (const std::exception& e)
    {
        Debug::Log("Exception in GeneratedBlankMaterial: %s\n", e.what());
        return false;
    }
}
