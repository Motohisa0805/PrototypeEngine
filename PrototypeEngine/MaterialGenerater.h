#pragma once
#pragma comment(lib, "Rpcrt4.lib")
#include "FBXImportSettings.h"
#include "Math.h"
#include "Renderer.h"
#include "Typedefs.h"
#include <assimp/scene.h>
#include <filesystem>
#include <windows.h>

namespace fs = std::filesystem;

struct MaterialParameters
{
    string sShader = "StandardPBR";
    float  sDiffuse_color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float  sAmbinet_color[3] = {1.0f, 1.0f, 1.0f};
    float  sShininess = 0.390625f;
    float  sMetallic = 0.0f;
    float  sRoughness = 0.0f;
    float  sEmissive[3]      = {0.0f, 0.0f, 0.0f};

    string sAlbedo_map = "";
    string sNormal_map = "";
    string sSpecular_map = "";
};

class MaterialGenerater
{
public:

	static fs::path GeneratedMatFilePath(const string& fileName);

	static bool		GeneratedBlankMaterial(const fs::path& path);

    static bool     GenerateMaterial(const fs::path& path, const MaterialParameters& params);
};
