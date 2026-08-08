#pragma comment(lib, "Rpcrt4.lib")
#include <windows.h>
#include <filesystem>
#include "Typedefs.h"
#include <assimp/scene.h>
#include "Math.h"
#include "FBXImportSettings.h"
#include "Renderer.h"

namespace fs = std::filesystem;

class AssetImporter
{
public:
    struct MeshBinHeader
    {
        uint32_t sLayoutType = 0;
        uint32_t sVertexCount = 0;
        uint32_t sIndexCount = 0;
        Vector3  sMin = Vector3();
        Vector3  sMax = Vector3();
        float    sColliderRadius = 0;
    };

    // スケルトンのバイナリデータ構造体
    struct SkeletonBinHeader
    {
        // ボーン名（固定長）
        char        sName[64];
        // 短縮版ボーン名
        char        sShortName[64];
        // 親ボーンインデックス（-1 なら root）
        int32_t     sParentIndex = 0;
        // バインドポーズの位置
        Vector3     sPosition = Vector3();
        // バインドポーズの回転
        Quaternion  sRotation = Quaternion();
        // バインドポーズのスケール（オプション）
        Vector3     sScale = Vector3();
    };

    struct AnimationBinHeader
    {
        uint32_t sVersion   = 1;
        float    sDuration  = 0;
        uint32_t sNumFrames = 0;
        uint32_t sNumBones  = 0;
    };

    struct AnimationBinTransform
    {
        Vector3    sPosition = Vector3();
        Quaternion sRotation = Quaternion();
        Vector3    sScale    = Vector3();
    };

private:
    static size_t                   FindTranslation(float AnimationTime, const aiNodeAnim* pNodeAnim);

    static void                     CalcInterpolatedTranslation(aiVector3D& Out,float AnimationTime,const aiNodeAnim* pNodeAnim);

    static size_t                   FindRotation(float AnimationTime,const aiNodeAnim* pNodeAnim);

    static void                     CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime,const aiNodeAnim* pNodeAnim);

    static size_t                   FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

    static void                     CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime,const aiNodeAnim* pNodeAnim);

public:
    // GUIから呼ばれる用の関数。Assetsフォルダ内の全てのFBXファイルをチェックして、必要であればインポートする
	static void                     CheckAndImportAssets();
    static void                     ReloadAssetsFile();
    // GUIから呼ばれる用の関数。1つのファイルをチェックして、必要であればインポートする
	static void                     OneFileCheckAndImportAssets(const filesystem::path& filePath);
    //名前の変更を行った時に再度読み込みを行う関数
    static void                     ReloadImportAssets(const filesystem::path& oldfilePath,const filesystem::path& newfilePath);

	static string                   GenerateUUID();

	static void                     ConvertFBXToCustomFormat(const fs::path& fbxPath,
                                         const fs::path& customPath);

	static void                     ExportMeshBinary(const fs::path& fbxPath,const fs::path& meshBinPath, int index);
	static void                     ExportSkeletonBinary(const aiScene* scene,const fs::path& skelBinPath);
	static void                     ExportAnimationBinary(const fs::path& fbxPath,const fs::path& animBinPath, int index);

    static AllImportSettings        OutputFBXMetaFile(const fs::path& fbxPath);

    static vector<string>           GetSubMeshNames(const fs::path& fbxPath);
    static vector<string>           GetSubMeshLocalID(const fs::path& fbxPath);
};
