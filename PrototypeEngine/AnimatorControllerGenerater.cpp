#include "AnimatorControllerGenerater.h"
#include "DebugManager.h"
#include "AssetImporter.h"

bool AnimatorControllerGenerater::GeneratedBlankController(const fs::path& path)
{
    AnimatorControllerParameters blankParams;
    blankParams.sDefaultState = "";
    //Entryノードの作成
    AnimState entryNode;
    entryNode.sStateName = "Entry";
    entryNode.sNodeType  = AnimNodeType::sEntry;
    entryNode.sPos       = Vector2(-200.0f, 0);
    blankParams.sStates.push_back(entryNode);
    //Any Stateノードの作成
    AnimState anyStateNode;
    anyStateNode.sStateName = "Any State";
    anyStateNode.sNodeType  = AnimNodeType::sAnyState;
    anyStateNode.sPos       = Vector2(-200.0f, -300.0f);
    blankParams.sStates.push_back(anyStateNode);
    //Exitノードの作成
    AnimState exitNode;
    exitNode.sStateName = "Exit";
    exitNode.sNodeType  = AnimNodeType::sExit;
    exitNode.sPos       = Vector2(600.0f, 0);
    blankParams.sStates.push_back(exitNode);

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
