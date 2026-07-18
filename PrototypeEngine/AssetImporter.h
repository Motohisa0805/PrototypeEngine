#pragma comment(lib, "Rpcrt4.lib")
#include <windows.h>
#include <filesystem>
#include "Typedefs.h"
#include <assimp/scene.h>
#include "Math.h"
#include "FBXImportSettings.h"

namespace fs = std::filesystem;

class AssetImporter
{
public:
    struct MeshBinHeader
    {
        uint32_t layoutType = 0;
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
        Vector3  min = Vector3();
        Vector3  max = Vector3();
        float    colliderRadius = 0;
    };

    // スケルトンのバイナリデータ構造体
    struct SkeletonBinHeader
    {
        // ボーン名（固定長）
        char name[64];
        // 短縮版ボーン名
        char shortName[64];
        // 親ボーンインデックス（-1 なら root）
        int32_t parentIndex = 0;
        // バインドポーズの位置
        Vector3 position = Vector3();
        // バインドポーズの回転
        Quaternion rotation = Quaternion();
        // バインドポーズのスケール（オプション）
        Vector3 scale = Vector3();
    };

    struct AnimationBinHeader
    {
        uint32_t version   = 1;
        float    duration  = 0;
        uint32_t numFrames = 0;
        uint32_t numBones  = 0;
    };

    struct AnimationBinTransform
    {
        Vector3    position = Vector3();
        Quaternion rotation = Quaternion();
        Vector3    scale    = Vector3();
    };

private:
    static size_t FindTranslation(float AnimationTime, const aiNodeAnim* pNodeAnim);

    static void CalcInterpolatedTranslation(aiVector3D&       Out,
                                            float             AnimationTime,
                                     const aiNodeAnim* pNodeAnim);

    static size_t FindRotation(float             AnimationTime,
                               const aiNodeAnim* pNodeAnim);

    static void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime,
                                  const aiNodeAnim* pNodeAnim);

    static size_t FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

    static void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime,
                                 const aiNodeAnim* pNodeAnim);

public:
    // GUIから呼ばれる用の関数。Assetsフォルダ内の全てのFBXファイルをチェックして、必要であればインポートする
	static void CheckAndImportAssets();
    // GUIから呼ばれる用の関数。1つのファイルをチェックして、必要であればインポートする
	static void OneFileCheckAndImportAssets(const filesystem::path& filePath);

	static string GenerateUUID();

	static fs::path GeneratedCustomPath(const fs::path& path);

	static void ConvertFBXToCustomFormat(const fs::path& fbxPath,
                                         const fs::path& customPath);

	static void ExportMeshBinary(const fs::path& fbxPath,const fs::path& meshBinPath, int index);
	static void ExportSkeletonBinary(const aiScene* scene,const fs::path& skelBinPath);
	static void ExportAnimationBinary(const fs::path& fbxPath,const fs::path& animBinPath, int index);

    static AllImportSettings OutputFBXMetaFile(const fs::path& fbxPath);
};
