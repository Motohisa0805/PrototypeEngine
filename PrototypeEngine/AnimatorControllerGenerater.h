#pragma once
#include "FBXImportSettings.h"
#include "Math.h"
#include "Renderer.h"
#include "Typedefs.h"
#include <assimp/scene.h>
#include <filesystem>
#include <windows.h>
#include "Animator.h"
#include "Animation.h"
#include "AnimatorControllerParameters.h"
#include <nlohmann/json.hpp>

namespace fs = filesystem;

//AnimatorControllerファイルのCRUD処理を行う
class AnimatorControllerGenerater
{
private:

public:
    static bool GeneratedBlankController(const fs::path& path);

    static bool GenerateController(const fs::path& path,const AnimatorControllerParameters& params);
};
