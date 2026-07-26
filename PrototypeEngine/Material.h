#pragma once
#include "Typedefs.h"
#include "Math.h"

/// <summary>
/// êßçÏíÜÉtÉ@ÉCÉã
/// </summary>

class Texture;

struct MaterialData
{
    string sShaderName = "StandardPBR";

    Vector4 sDiffuseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    Vector3 sAmbientColor = Vector3(0.2f, 0.2f, 0.2f);
    Vector3 sSpecularColor = Vector3(0.5f, 0.5f, 0.5f);
    float   sShininess     = 0.390625f;

    float   sMetallic = 0;
    float   sRoughness = 0;
    Vector3 sEmissive = Vector3();

    string  sAlbedoTexturePath = "";
};

class Material
{
private:
    MaterialData    mData;

    string          mFilePath;

    Texture*        mAlbedoTexture;
public:
    Material();
    ~Material();    

    bool            LoadFromFile(const string& filePath);
    bool            SaveToFile(const string& filePath);

    MaterialData&   GetData() { return mData; }

    Texture*        GetTexture() { return mAlbedoTexture; }
    void            SetTexture(Texture* texture) { mAlbedoTexture = texture; }
    const string&   GetFilePath() const { return mFilePath; }

};
