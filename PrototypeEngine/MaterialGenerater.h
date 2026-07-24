#pragma comment(lib, "Rpcrt4.lib")
#include "FBXImportSettings.h"
#include "Math.h"
#include "Renderer.h"
#include "Typedefs.h"
#include <assimp/scene.h>
#include <filesystem>
#include <windows.h>

namespace fs = std::filesystem;

class MaterialGenerater
{
public:

	static fs::path GeneratedMatFilePath(const string& fileName);

	static bool		GeneratedBlankMaterial(const fs::path& path);
};
