#include "AnimatorControllerGenerater.h"
#include "DebugManager.h"
#include "AssetImporter.h"

bool AnimatorControllerGenerater::GeneratedBlankController(const fs::path& path)
{
    AnimatorControllerParameters blankParams;
    blankParams.sDefaultState = "";
    return GenerateController(path, blankParams);
}

bool AnimatorControllerGenerater::GenerateController(
    const fs::path& path, const AnimatorControllerParameters& params)
{
    try
    {
        nlohmann::json controllerJson = params;

        if (path.has_parent_path() && !fs::exists(path.parent_path()))
        {
            fs::create_directories(path.parent_path());
        }

        std::ofstream outFile(path);
        if (!outFile.is_open())
        {
            return false;
        }

        outFile << std::setw(4) << controllerJson << std::endl;
        //メタファイルの作成
        AssetImporter::OneFileCheckAndImportAssets(path);
        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }
}
