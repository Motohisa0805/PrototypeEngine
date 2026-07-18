#include "AssetImporter.h"
#include "Math.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "DebugManager.h"
#include "VertexArray.h"
#include "Collision.h"

void AssetImporter::CheckAndImportAssets()
{ 
	string assetsDir = "Assets/";

	for (const auto& entry : fs::recursive_directory_iterator(assetsDir)) {
		if (entry.is_regular_file() && entry.path().extension() == ".fbx") {
            fs::path fbxPath = entry.path();
			//対応する独自ファイル
            fs::path customPath = GeneratedCustomPath(fbxPath);
			//独自ファイルが存在しない、またはFBXファイルの方が新しく更新されている場合
			if (!fs::exists(customPath) || fs::last_write_time(entry) > fs::last_write_time(customPath)){
				//ここでFBXを読み込み、独自ファイルへ書き出す処理を呼ぶ
                ConvertFBXToCustomFormat(fbxPath, customPath);
            }
		}
	}
}

void AssetImporter::OneFileCheckAndImportAssets(
    const filesystem::path& filePath)
{
    string assetsDir = "Assets/";

    for (const auto& entry : fs::recursive_directory_iterator(assetsDir))
    {
        if (entry.is_regular_file() && entry.path() == filePath)
        {
            fs::path fbxPath = entry.path();
            // 対応する独自ファイル
            fs::path customPath = GeneratedCustomPath(fbxPath);
            // 独自ファイルが存在しない、またはFBXファイルの方が新しく更新されている場合
            if (!fs::exists(customPath) ||
                fs::last_write_time(entry) > fs::last_write_time(customPath))
            {
                // ここでFBXを読み込み、独自ファイルへ書き出す処理を呼ぶ
                ConvertFBXToCustomFormat(fbxPath, customPath);

            }
        }
    }
}

string AssetImporter::GenerateUUID()
{
    UUID uuid;
    // バイナリ形式のUUIDを生成
    if (UuidCreate(&uuid) != RPC_S_OK)
    {
        return ""; // UUID生成に失敗した場合は空文字を返す
    }

    RPC_CSTR uuidStr = nullptr;
    if (UuidToStringA(&uuid, &uuidStr) != RPC_S_OK)
    {
        return "";
    }
    
    string result(reinterpret_cast<char*>(uuidStr));

    // メモリを解放
    RpcStringFreeA(&uuidStr);

    return result; 
}

fs::path AssetImporter::GeneratedCustomPath(const fs::path& path)
{
    return path.parent_path() / (path.filename().string() + ".meta");
}

void AssetImporter::ConvertFBXToCustomFormat(const fs::path& fbxPath,
                                             const fs::path& customPath)
{
    nlohmann::json metaJson;
    bool isNewFile = !fs::exists(customPath);

    if (!isNewFile)
    {
        std::ifstream inFile(customPath);
        if (inFile.is_open())
        {
            inFile >> metaJson;
            inFile.close();
        }
    }

    //AssimpでFBXの構造だけを軽くスキャンする
    Assimp::Importer importer;
    //頂点データなどは重いため、構造だけを読むフラグで読み込み
    const aiScene* scene = importer.ReadFile(fbxPath.string(), 0);

    bool hasMesh = scene && scene->HasMeshes();
    bool hasAnim = scene && scene->HasAnimations();
    bool hasBones = false;

    if (isNewFile)
    {
        metaJson["fileFormatVersion"] = 1;
        metaJson["guid"]              = GenerateUUID();
        // インポートスイッチ(中身の有無によって自動でON/OFFを設定)
        metaJson["import_settings"]["import_mesh"]      = hasMesh;
        metaJson["import_settings"]["import_skeleton"]  = hasBones;
        metaJson["import_settings"]["import_animation"] = hasAnim;

        // スケールやその他の共通設定
        metaJson["import_settings"]["scale_factor"] = 1.0f;
        metaJson["skeleton_settings"] = {{"skeleton_type", 0}};
    }

    //メッシュ情報を記録
    nlohmann::json meshsJson = nlohmann::json::array();
    if (hasMesh)
    {
        for (unsigned int i = 0; i < scene->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[i];
            nlohmann::json meshInfo;
            meshInfo["name"] = mesh->mName.C_Str();
            meshInfo["vertex_count"] = mesh->mNumVertices;
            meshInfo["has_bones"]    = mesh->HasBones();
            
            string meshBinName = fbxPath.stem().string() + "_mesh" + std::to_string(i) + ".meshbin";
            meshInfo["binary_path"] = meshBinName;


            meshsJson.push_back(meshInfo);

            if (mesh->HasBones())
            {
                hasBones = true;
            }
        }
    }
    metaJson["cached_data"]["meshes"] = meshsJson;

    //アニメーション情報の記録
    nlohmann::json animsJson = nlohmann::json::array();
    if (hasAnim)
    {
        for (unsigned int i = 0; i < scene->mNumAnimations; i++)
        {
            aiAnimation* anim = scene->mAnimations[i];
            nlohmann::json animInfo;

            animInfo["clip_name"] = anim->mName.C_Str();
            animInfo["duration"]  = anim->mDuration;

            float ticks =
                anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0f;
            animInfo["ticks_per_second"] = ticks;

            //最大フレーム数の計算
            unsigned int numFrames = 0;
            for (unsigned int c = 0; c < anim->mNumChannels; c++)
            {
                aiNodeAnim* channel = anim->mChannels[c];
                numFrames = Math::Max(numFrames, channel->mNumPositionKeys);
                numFrames = Math::Max(numFrames, channel->mNumRotationKeys);
                numFrames = Math::Max(numFrames, channel->mNumScalingKeys);
            }
            animInfo["num_frames"] = numFrames;

            //ユーザーがいじれる設定項目
            nlohmann::json animSettings;
            animSettings["is_looping"] = true;
            animSettings["is_root_motion"] = false;
            animSettings["start_frame"] = 0;
            animSettings["end_frame"] = numFrames > 0 ? numFrames - 1 : 0;
            animInfo["settings"] = animSettings;

            string animBinName = fbxPath.stem().string() + "_anim" + std::to_string(i) + ".animbin";
            animInfo["binary_path"] = animBinName;

            animsJson.push_back(animInfo);
        }
    }
    metaJson["cached_data"]["animations"] = animsJson;

    // メッシュのバイナリデータを書き出す
    if (metaJson["import_settings"]["import_mesh"] == true && hasMesh)
    {
        for (unsigned int i = 0; i < scene->mNumMeshes; i++)
        {
            string meshBinName = metaJson["cached_data"]["meshes"][i]["binary_path"];
            fs::path meshBinPath = (filesystem::path)"Binary/mesh" / meshBinName;
            ExportMeshBinary(fbxPath,meshBinPath,i);
        }
    }

    //スケルトンバイナリの書き出し
    if (metaJson["import_settings"]["import_skeleton"] == true && hasBones)
    {
        fs::path skelBinPath = (filesystem::path)"Binary/skeleton" / (fbxPath.stem().string() + ".skelbin");
        ExportSkeletonBinary(scene, skelBinPath);
    }

    //アニメーションバイナリの書き出し
    if (metaJson["import_settings"]["import_animation"] == true && hasAnim)
    {
        for (unsigned int i = 0; i < scene->mNumAnimations; i++)
        {
            string animBinName = metaJson["cached_data"]["animations"][i]["binary_path"];
            fs::path animBinPath = (filesystem::path)"Binary/anim" / animBinName;
            ExportAnimationBinary(fbxPath, animBinPath, i);
        }
    }

    // JSONをファイルに書き込む
    std::ofstream outFile(customPath);
    if (outFile.is_open())
    {
        outFile << metaJson.dump(4); // インデント幅を4に設定して整形
        outFile.close();
    }
    else
    {
        std::cerr << "Failed to open file for writing: " << customPath << std::endl;
    }
}

void AssetImporter::ExportMeshBinary(const fs::path& fbxPath,
                                     const fs::path& meshBinPath, int index)
{
    // モデル情報取得
    Assimp::Importer importer;
    const aiScene*   scene = importer.ReadFile(
        fbxPath.string(),
        aiProcess_Triangulate | aiProcess_FlipUVs |
        aiProcess_GenNormals | aiProcess_GlobalScale |
        aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder
    );

    // MeshCheck
    if (!scene || !scene->HasMeshes() || index >= (int)scene->mNumMeshes)
    {
        Debug::Log("Assimp Error: %s", importer.GetErrorString());
        return;
    }

    // メッシュをロード
    aiMesh* mesh = scene->mMeshes[index];

    vector<Vertex> vertices;
    vector<uint32_t> indices;

    float radius = 0.0f;
    AABB  box    = AABB(Vector3::Infinity, Vector3::NegInfinity);

    // 頂点データの変換
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        aiVector3D pos  = mesh->mVertices[i];
        aiVector3D norm = mesh->mNormals[i];
        aiVector3D uv = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i]
                                                  : aiVector3D(0, 0, 0);

        Vector3 vertexPos(pos.x, pos.y, pos.z);
        radius = Math::Max(radius, vertexPos.LengthSq());
        box.UpdateMinMax(vertexPos);

        Vertex v;
        v.pos.x = pos.x;
        v.pos.y = pos.y;
        v.pos.z = pos.z;

        if (mesh->HasNormals())
        {
            v.normal.x = norm.x;
            v.normal.y = norm.y;
            v.normal.z = norm.z;
        }

        if (mesh->HasTextureCoords(0))
        {
            v.uv.x = uv.x;
            v.uv.y = uv.y;
        }

        vertices.push_back(v);
    }
    radius = Math::Sqrt(radius);

    // 頂点ループが終わったあと
    Vector3    center   = (box.mMin + box.mMax) * 0.5f;
    Vector3    extents  = (box.mMax - box.mMin) * 0.5f;
    Quaternion rotation = Quaternion::Identity;
    OBB        obbBox(center, rotation, extents);

    // **インデックスデータの変換**
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        if (face.mNumIndices == 3)
        {
            indices.emplace_back(face.mIndices[0]);
            indices.emplace_back(face.mIndices[1]);
            indices.emplace_back(face.mIndices[2]);
        }
    }

    // Skinの場合のLayout変更
    VertexArray::Layout layout = mesh->HasBones() ?  VertexArray::PosNormSkinTex : VertexArray::PosNormTex;

    // バイナリに変換
    MeshBinHeader header;
    header.layoutType  = (layout == VertexArray::PosNormTex) ? 0 : 1;
    header.vertexCount = static_cast<uint32_t>(vertices.size());
    header.indexCount  = static_cast<uint32_t>(indices.size());
    header.min            = box.mMin;
    header.max            = box.mMax;
    header.colliderRadius = radius; // 半径計算済みと仮定

    std::ofstream out(meshBinPath,std::ios::binary);
    if (out.is_open())
    {
        out.write((char*)&header, sizeof(header));
        out.write((char*)vertices.data(), sizeof(Vertex) * vertices.size());
        out.write((char*)indices.data(), sizeof(uint32_t) * indices.size());
        out.close();
        Debug::Log("Successfully exported mesh binary:%s", meshBinPath);
    }
    else
    {
        Debug::Log("Failed to open mesh binary path for writing:%s", meshBinPath);
    }
}

void AssetImporter::ExportSkeletonBinary(const aiScene*  scene,
                                         const fs::path& skelBinPath)
{
    struct TempBone
    {
        string name;
        int    parentIndex = -1;
        Vector3 position;
        Quaternion rotation;
        Vector3    scale;
    };

    vector<TempBone> tempBones;
    std::unordered_map<string, int> boneNameToIndex;

        // メッシュの数でfor文
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];
        // メッシュ内のボーン数でfor文
        for (unsigned int j = 0; j < mesh->mNumBones; j++)
        {
            // ボーンを取得
            aiBone* bone = mesh->mBones[j];
            // ボーン名を取得
            string boneName = bone->mName.C_Str();
            // boneNameToIndexにすでに同じボーンがないかチェック
            if (boneNameToIndex.find(boneName) != boneNameToIndex.end())
                continue;

            TempBone b;
            b.name = boneName;

            // バインドポーズの変換
            // ボーンのmOffsetMatrix取得
            aiMatrix4x4  bindPose = bone->mOffsetMatrix;
            aiVector3D   pos;
            aiQuaternion rot;
            aiVector3D   scale;
            // ボーンのバインドポーズを各値に分解
            bindPose.Decompose(scale, rot, pos);

            b.position = Vector3(pos.x, pos.y, pos.z);
            b.rotation = Quaternion(rot.x, rot.y, rot.z, rot.w);
            b.scale    = Vector3(scale.x, scale.y, scale.z);

            boneNameToIndex[boneName] = static_cast<int>(tempBones.size());
            tempBones.push_back(b);
        }
    }

    auto SetParentBonesLambda = [&](auto& self, aiNode* node,
                                    int parentIndex) -> void
    { 
        string nodeName = node->mName.C_Str();
        int    currentIndex = parentIndex;
        
        if (boneNameToIndex.find(nodeName) != boneNameToIndex.end())
        {
            currentIndex = boneNameToIndex[nodeName];
            tempBones[currentIndex].parentIndex = parentIndex;
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            self(self, node->mChildren[i], currentIndex);
        }
    };

    if (scene->mRootNode != nullptr)
    {
        SetParentBonesLambda(SetParentBonesLambda, scene->mRootNode, -1);
    }

    std::ofstream out(skelBinPath,std::ios::binary);
    if (!out)
    {
        Debug::ErrorLog("Failed to open skelbin for writing:%s",skelBinPath.string().c_str());
        return;
    }

    uint32_t boneCount = static_cast<uint32_t>(tempBones.size());
    out.write((char*)&boneCount, sizeof(uint32_t));

    for (const auto& b : tempBones)
    {
        SkeletonBinHeader bin{};
        strncpy_s(bin.name, b.name.c_str(),64);
        bin.parentIndex = b.parentIndex;
        bin.position    = b.position;
        bin.rotation    = b.rotation;
        bin.scale       = b.scale;

        out.write((char*)&bin, sizeof(SkeletonBinHeader));
    }
    Debug::Log("Successfully exported skeleton binary: %s",skelBinPath.string().c_str());
    out.close();
}

void AssetImporter::ExportAnimationBinary(const fs::path& fbxPath,
                                          const fs::path& animBinPath,
                                          int             index)
{
    Assimp::Importer importer;
    const aiScene*   scene = importer.ReadFile(
        fbxPath.string(), aiProcess_Triangulate | aiProcess_LimitBoneWeights |
                      aiProcess_GlobalScale | aiProcess_MakeLeftHanded |
                      aiProcess_FlipUVs | aiProcess_FlipWindingOrder);

    if (!scene || !scene->HasAnimations())
    {
        Debug::Log("No animations found in FBX: %s", fbxPath.c_str());
        return;
    }

    aiAnimation* anim = scene->mAnimations[index];

    float ticksPerSecond = (anim->mTicksPerSecond != 0) ? anim->mTicksPerSecond : 25.0f;
    int duration            = static_cast<float>(anim->mDuration / ticksPerSecond);

    // mNumFrames をキーの最大値に合わせる
    unsigned int numFrames = 0;
    for (unsigned int i = 0; i < anim->mNumChannels; i++)
    {
        aiNodeAnim* channel = anim->mChannels[i];
        numFrames =
            Math::Max((unsigned int)numFrames, channel->mNumPositionKeys);
        numFrames =
            Math::Max((unsigned int)numFrames, channel->mNumRotationKeys);
        numFrames =
            Math::Max((unsigned int)numFrames, channel->mNumScalingKeys);
    }

    //ボーンマップの作成
    std::unordered_map<string, int> boneNameToIndex;
    vector<string>                  boneNames;

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumBones; j++)
        {
            string boneName = mesh->mBones[j]->mName.C_Str();
            if (boneNameToIndex.find(boneName) == boneNameToIndex.end())
            {
                boneNameToIndex[boneName] = static_cast<int>(boneNames.size());
                boneNames.push_back(boneName);
            }
        }
    }

    uint32_t numBones = static_cast<uint32_t>(boneNames.size());

    vector <vector<AnimationBinTransform>> tracks(numBones);
    for (auto& track : tracks)
    {
        track.resize(numBones);
    }

    //初期ポーズ(バインドポーズ)で全フレームを埋める処理
    auto SetFallbackBindPose = [&](auto& self, aiNode* node) -> void{ 
        string nodeName = node->mName.C_Str();
        if (boneNameToIndex.find(nodeName) != boneNameToIndex.end())
        {
            int bIndex = boneNameToIndex[nodeName];

            aiVector3D scale, pos;
            aiQuaternion rot;
            node->mTransformation.Decompose(scale, rot, pos);

            AnimationBinTransform bindTransform;
            bindTransform.position = Vector3(pos.x, pos.y, pos.z);
            bindTransform.rotation = Quaternion(rot.x, rot.y, rot.z, rot.w);
            bindTransform.scale    = Vector3(scale.x, scale.y, scale.z);

            for (unsigned int f = 0; f < numFrames; ++f)
            {
                tracks[bIndex][f] = bindTransform;
            }
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            self(self, node->mChildren[i]);
        }
    };
    if (scene->mRootNode)
        SetFallbackBindPose(SetFallbackBindPose, scene->mRootNode);

    // アニメーションキーのフレームのサンプリング
    for (unsigned int i = 0; i < anim->mNumChannels; i++)
    {
        aiNodeAnim* channel  = anim->mChannels[i];
        string      boneName = channel->mNodeName.C_Str();

        if (boneNameToIndex.find(boneName) == boneNameToIndex.end())continue;
        int boneIndex = boneNameToIndex[boneName];

        for (unsigned int f = 0; f < numFrames; f++)
        {
            float timeInTicks =
                (float)f * (anim->mDuration / (float)(numFrames > 1 ? numFrames - 1 : 1));

            aiVector3D pos;
            CalcInterpolatedTranslation(pos, timeInTicks, channel);

            // 回転キーの適用
            aiQuaternion rot;
            CalcInterpolatedRotation(rot, timeInTicks, channel);

            // スケールキーの適用
            aiVector3D scale;
            CalcInterpolatedScaling(scale, timeInTicks, channel);

            tracks[boneIndex][f].position = Vector3(pos.x, pos.y, pos.z);
            tracks[boneIndex][f].rotation = Quaternion(rot.x, rot.y, rot.z, rot.w);
            tracks[boneIndex][f].scale    = Vector3(scale.x, scale.y, scale.z);
        }
    }

    std::ofstream out(animBinPath, std::ios::binary);
    if (!out)
    {
        Debug::Log("Failed to open animation bin for writing: %s",animBinPath.c_str());
        return;
    }

    AnimationBinHeader header;
    header.duration  = duration;
    header.numFrames = static_cast<uint32_t>(numFrames);
    header.numBones  = static_cast<uint32_t>(numBones);
    out.write((char*)&header, sizeof(header));

    for (size_t bone = 0; bone < numBones; ++bone)
    {
        for (size_t frame = 0; frame < numFrames; ++frame)
        {
            out.write((char*)&tracks[bone][frame], sizeof(AnimationBinTransform));
        }
    }
    out.close();
    Debug::Log("Successfully exported animation binary: %s",animBinPath.string().c_str());
}

AllImportSettings AssetImporter::OutputFBXMetaFile(const fs::path& fbxPath)
{
    // 対応する独自ファイル
    fs::path customPath = GeneratedCustomPath(fbxPath);

    nlohmann::json metaJson;
    bool           isActive = fs::exists(customPath);

    if (!isActive) return AllImportSettings();

    std::ifstream inFile(customPath);
    if (inFile.is_open())
    {
        inFile >> metaJson;
        inFile.close();
    }

    //各データを読み込み
    AllImportSettings importData;

    if (metaJson.contains("import_settings"))
    {
        auto& settings = metaJson["import_settings"];
        importData.sModel.sScaleFactory = settings.value("import_settings",1.0f);
    }

    return importData;
}

// 補間情報の計算、チュートリアルから引用
//  https://ogldev.org/www/tutorial38/tutorial38.html
size_t AssetImporter::FindTranslation(float             AnimationTime,
                                  const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (size_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
    {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
        {
            return i;
        }
    }

    assert(0);
    return 0;
}
void AssetImporter::CalcInterpolatedTranslation(aiVector3D&       Out,
                                            float             AnimationTime,
                                            const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1)
    {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }
    if (AnimationTime >=
        pNodeAnim->mPositionKeys[pNodeAnim->mNumPositionKeys - 1].mTime)
    {
        Out = pNodeAnim->mPositionKeys[pNodeAnim->mNumPositionKeys - 1].mValue;
        return;
    }

    size_t TranslationIndex = FindTranslation(AnimationTime, pNodeAnim);

    size_t NextTranslationIndex = (TranslationIndex + 1);

    assert(NextTranslationIndex < pNodeAnim->mNumPositionKeys);

    float DeltaTime = pNodeAnim->mPositionKeys[NextTranslationIndex].mTime -
                      pNodeAnim->mPositionKeys[TranslationIndex].mTime;

    float Factor = (AnimationTime -
                    (float)pNodeAnim->mPositionKeys[TranslationIndex].mTime) /
                   DeltaTime;

    assert(Factor >= 0.0f && Factor <= 1.0f);

    const aiVector3D& StartTranslation =
        pNodeAnim->mPositionKeys[TranslationIndex].mValue;

    const aiVector3D& EndTranslation =
        pNodeAnim->mPositionKeys[NextTranslationIndex].mValue;

    Out = StartTranslation + (EndTranslation - StartTranslation) * Factor;
}

void AssetImporter::CalcInterpolatedRotation(aiQuaternion&     Out,
                                             float             AnimationTime,
                                         const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumRotationKeys == 1)
    {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }
    if (AnimationTime >=
        pNodeAnim->mRotationKeys[pNodeAnim->mNumRotationKeys - 1].mTime)
    {
        Out = pNodeAnim->mRotationKeys[pNodeAnim->mNumRotationKeys - 1].mValue;
        return;
    }

    size_t RotationIndex = FindRotation(AnimationTime, pNodeAnim);

    size_t NextRotationIndex = (RotationIndex + 1);

    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);

    float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime -
                      pNodeAnim->mRotationKeys[RotationIndex].mTime;

    float Factor =
        (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) /
        DeltaTime;

    assert(Factor >= 0.0f && Factor <= 1.0f);

    const aiQuaternion& StartRotationQ =
        pNodeAnim->mRotationKeys[RotationIndex].mValue;

    const aiQuaternion& EndRotationQ =
        pNodeAnim->mRotationKeys[NextRotationIndex].mValue;

    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);

    Out = Out.Normalize();
}
size_t AssetImporter::FindRotation(float             AnimationTime,
                                   const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (size_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
    {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
        {
            return i;
        }
    }

    assert(0);
    return 0;
}

void AssetImporter::CalcInterpolatedScaling(aiVector3D&       Out,
                                            float             AnimationTime,
                                        const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1)
    {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }
    if (AnimationTime >=
        pNodeAnim->mScalingKeys[pNodeAnim->mNumScalingKeys - 1].mTime)
    {
        Out = pNodeAnim->mScalingKeys[pNodeAnim->mNumScalingKeys - 1].mValue;
        return;
    }

    size_t ScalingIndex = FindScaling(AnimationTime, pNodeAnim);

    size_t NextScalingIndex = (ScalingIndex + 1);

    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);

    float DeltaTime = pNodeAnim->mScalingKeys[NextScalingIndex].mTime -
                      pNodeAnim->mScalingKeys[ScalingIndex].mTime;

    float Factor =
        (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) /
        DeltaTime;

    assert(Factor >= 0.0f && Factor <= 1.0f);

    const aiVector3D& StartScaling =
        pNodeAnim->mScalingKeys[ScalingIndex].mValue;

    const aiVector3D& EndScaling =
        pNodeAnim->mScalingKeys[NextScalingIndex].mValue;

    Out = StartScaling + (EndScaling - StartScaling) * Factor;
}
size_t AssetImporter::FindScaling(float             AnimationTime,
                                  const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumPositionKeys > 0);

    for (size_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
    {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
        {
            return i;
        }
    }
    assert(0);
    return 0;
}