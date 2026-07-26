#include "Material.h"
#include "Texture.h"
#include "FilePath.h"
#include "EngineWindow.h"
#include "Renderer.h"
#include <fstream>
#include <nlohmann/json.hpp>

Material::Material() 
:mData()
,mFilePath(""), mAlbedoTexture(nullptr) {}

Material::~Material() 
{
	if (mAlbedoTexture)
	{
        mAlbedoTexture = nullptr;
	}
}

bool Material::LoadFromFile(const string& filePath) 
{
    std::ifstream matFile(filePath);
    if (!matFile.is_open())return false;

    nlohmann::json matJson;
    try
    {
        matFile >> matJson;

        if (matJson.contains("shader"))
        {
            mData.sShaderName = matJson["shader"];
        }

        //プロパティ
        if (matJson.contains("properties"))
        {
            auto props = matJson["properties"];

            if (props.contains("diffuse_color"))
            {
                auto c = props["diffuse_color"];
                mData.sDiffuseColor = Vector4(c[0], c[1], c[2], c[3]);
            }
            if (props.contains("ambient_color"))
            {
                auto c = props["ambient_color"];
                mData.sAmbientColor = Vector3(c[0], c[1], c[2]);
            }
            if (props.contains("specular_color"))
            {
                auto c = props["specular_color"];
                mData.sSpecularColor = Vector3(c[0], c[1], c[2]);
            }
            mData.sShininess = props.value("shininess", 0.390625f);

            mData.sMetallic = props.value("metallic", 0);
            mData.sRoughness = props.value("roughness", 0);
            if (props.contains("emissive"))
            {
                auto c = props["emissive"];
                mData.sEmissive = Vector3(c[0], c[1], c[2]);
            }
        }

        //テクスチャ
        if (matJson.contains("textures"))
        {
            mData.sAlbedoTexturePath = matJson["textures"].value("albedo_map", "");

            //パスがあればテクスチャをロード
            if (!mData.sAlbedoTexturePath.empty())
            {
                if (!mAlbedoTexture)
                {
                    mAlbedoTexture = EngineWindow::GetRenderer()->GetTexture(mData.sAlbedoTexturePath);
                }
            }
        }
        mFilePath = filePath;
        return true; 
    }
    catch (...)
    {
        return false; 
    }

}

bool Material::SaveToFile(const string& filePath) 
{
    nlohmann::json matJson;

    //シェーダー名
    matJson["shader"] = mData.sShaderName;

    //プロパティ
    matJson["properties"]["diffuse_color"] = { mData.sDiffuseColor.x, mData.sDiffuseColor.y, mData.sDiffuseColor.z, mData.sDiffuseColor.w };
    matJson["properties"]["ambient_color"] = { mData.sAmbientColor.x, mData.sAmbientColor.y, mData.sAmbientColor.z};
    matJson["properties"]["specular_color"] = { mData.sSpecularColor.x, mData.sSpecularColor.y, mData.sSpecularColor.z};
    matJson["properties"]["shininess"] = mData.sShininess;

    matJson["properties"]["metallic"] = mData.sMetallic;
    matJson["properties"]["roughness"] = mData.sRoughness;
    matJson["properties"]["emissive"] = { mData.sEmissive.x, mData.sEmissive.y, mData.sEmissive.z};

    //テクスチャ
    matJson["textures"]["albedo_map"] = mData.sAlbedoTexturePath;

    //ファイルへの書き出し
    std::ofstream file(filePath);
    if (file.is_open())
    {
        file << matJson.dump(4);
        return true;
    }

    return false; 
}
