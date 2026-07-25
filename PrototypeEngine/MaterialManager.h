#pragma once
#include "Material.h"
#include "Typedefs.h"

class MaterialManager
{
private:
	//ロード済みのマテリアルを保持する連想配列
    static std::unordered_map<string, Material*> mMaterials;

public:
	//マテリアルを取得する
    static Material* GetMaterial(const string& filePath);

	//エンジン終了時にメモリを解放
    static void AllMaterialClear();
};
