#include "MaterialManager.h"
#include "DebugManager.h"
std::unordered_map<string, Material*> MaterialManager::mMaterials; 

Material* MaterialManager::GetMaterial(const string& filePath)
{
    //既にキャッシュに存在するなら
    auto iter = mMaterials.find(filePath);
    if (iter != mMaterials.end())
    {
        return iter->second;
    }

    //存在しないなら新しくロード
    Material* newMat = new Material();
    if (newMat->LoadFromFile(filePath))
    {
        mMaterials[filePath] = newMat;
        return newMat;
    }
    else
    {
        delete newMat;
        Debug::WarningLog("Failed to load material: %s", filePath.c_str());
        return nullptr;
    }
}

void MaterialManager::AllMaterialClear() 
{ 
    for (auto& pair : mMaterials)
    {
        delete pair.second;
    }
    mMaterials.clear();
}
