#include "AssetImporter.h"
#include "Math.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "DebugManager.h"
#include "VertexArray.h"
#include "Collision.h"
#include <stack>
#include "AssetDataBase.h"

//起動時に一回呼び出す
void AssetImporter::CheckAndImportAssets()
{ 
	string assetsDir = "Assets/";

	for (const auto& entry : fs::recursive_directory_iterator(assetsDir)) {
		if (entry.is_regular_file() && entry.path().extension() == ".fbx") {
            fs::path fbxPath = entry.path();
			//対応する独自ファイル
            fs::path customPath = AssetDataBase::GetInstance().GeneratedMetaFilePath(fbxPath);
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
    if (fs::exists(filePath) && filePath.extension() == ".fbx")
    {
        // 対応する独自ファイル
        fs::path customPath = AssetDataBase::GetInstance().GeneratedMetaFilePath(filePath);
        // 独自ファイルが存在しない、またはFBXファイルの方が新しく更新されている場合
        if (!fs::exists(customPath) ||
            fs::last_write_time(filePath) > fs::last_write_time(customPath))
        {
            // ここでFBXを読み込み、独自ファイルへ書き出す処理を呼ぶ
            ConvertFBXToCustomFormat(filePath, customPath);
        }
    }
}

void AssetImporter::ReloadImportAssets(const filesystem::path& oldfilePath,
                                       const filesystem::path& newfilePath)
{
    nlohmann::json metaJson;

    if (!fs::exists(oldfilePath))
    {
        return;
    }

    std::ifstream inFile(oldfilePath);
    if (inFile.is_open())
    {
        inFile >> metaJson;
        inFile.close();
    }
    else
    {
        Debug::Log("Failed to open old meta file: %s",oldfilePath.string().c_str());
        return;
    }

    string oldBaseName = oldfilePath.stem().stem().string();
    string newBaseName = newfilePath.stem().stem().string();

    if (metaJson.contains("cached_data"))
    {
        //メッシュバイナリの名前変更とJSON内のパス書き換え
        if (metaJson["cached_data"].contains("meshes"))
        {
            auto& meshes = metaJson["cached_data"]["meshes"];
            for (size_t i = 0; i < meshes.size(); i++)
            {
                if (meshes[i].contains("binary_path"))
                {
                    string   oldBinName = meshes[i]["binary_path"];
                    fs::path oldBinPath = fs::path("Binary/mesh") / oldBinName;

                    string newBinName = newBaseName + "_mesh" + std::to_string(i) + ".meshbin";
                    fs::path newBinPath = fs::path("Binary/mesh") / newBinName;

                    if (fs::exists(oldBinPath))
                    {
                        fs::rename(oldBinPath, newBinPath);
                    }

                    //JSON内の値を更新
                    meshes[i]["binary_path"] = newBinName;
                }
            }
        }

        // アニメーションバイナリの名前変更とJSON内のパス書き換え
        if (metaJson["cached_data"].contains("animations"))
        {
            auto& animations = metaJson["cached_data"]["animations"];
            for (size_t i = 0; i < animations.size(); i++)
            {
                if (animations[i].contains("binary_path"))
                {
                    string   oldBinName = animations[i]["binary_path"];
                    fs::path oldBinPath = fs::path("Binary/anim") / oldBinName;

                    string newBinName =
                        newBaseName + "_mesh" + std::to_string(i) + ".meshbin";
                    fs::path newBinPath = fs::path("Binary/anim") / newBinName;

                    if (fs::exists(oldBinPath))
                    {
                        fs::rename(oldBinPath, newBinPath);
                    }

                    // JSON内の値を更新
                    animations[i]["binary_path"] = newBinName;
                }
            }
        }
    }

    //スケルトンの名前変更
    fs::path oldSkelPath = fs::path("Binary/skeleton") / (oldBaseName + ".skelbin");
    fs::path newSkelPath = fs::path("Binary/skeleton") / (newBaseName + ".skelbin");
    if (fs::exists(oldSkelPath))
    {
        fs::rename(oldSkelPath,newSkelPath);
    }

    //書き換えたJSONを新しい.metaファイルとして保存
    std::ofstream outFile(newfilePath);
    if (outFile.is_open())
    {
        outFile << metaJson.dump(4);
        outFile.close();
    }
    else
    {
        Debug::Log("Failed to write updated meta file: %s",newfilePath.string().c_str());
    }
    //古い.metaファイルを削除
    fs::remove(oldfilePath);

    Debug::Log("Successfully renamed asset from %s to %s", oldBaseName.c_str(),newBaseName.c_str());
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
    const aiScene* scene = importer.ReadFile(
        fbxPath.string(),
        aiProcess_Triangulate | aiProcess_FlipUVs |
        aiProcess_GenNormals | aiProcess_GlobalScale |
        aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder);

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

    vector<string> meshLocalIDs;

    //メッシュ情報を記録
    nlohmann::json meshsJson = nlohmann::json::array();
    if (hasMesh)
    {
        meshLocalIDs.resize(scene->mNumMeshes);
        for (unsigned int i = 0; i < scene->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[i];
            nlohmann::json meshInfo;

            // 既存の.metaファイルが存在し、LocalIDが設定されている場合はそれを使用する
            string localID;
            if (!isNewFile && metaJson.contains("cached_data") &&
                metaJson["cached_data"].contains("meshes") &&
                metaJson["cached_data"]["meshes"].size() > i)
            {
                localID = metaJson["cached_data"]["meshes"][i].value("localID",GenerateUUID());
            }
            else
            {
                localID = GenerateUUID();
            }
            meshLocalIDs[i]          = localID;
            meshInfo["localID"]      = localID;
            meshInfo["name"] = mesh->mName.C_Str();
            meshInfo["vertex_count"] = mesh->mNumVertices;
            meshInfo["has_bones"]    = mesh->HasBones();
            //マテリアルインデックス
            meshInfo["material_index"] = mesh->mMaterialIndex;

            //インデックス(ポリゴン)数の計算
            uint32_t indexCount = 0;
            for (unsigned int f = 0; f < mesh->mNumFaces; f++)
            {
                indexCount += mesh->mFaces[i].mNumIndices;
            }
            meshInfo["index_count"] = indexCount;

            //AABBとバウンディング半径の計算
            float radiusSq = 0.0f;
            AABB  box      = AABB(Vector3::Infinity, Vector3::NegInfinity);
            for (unsigned int v = 0; v < mesh->mNumVertices; v++)
            {
                Vector3 pos(mesh->mVertices[v].x, mesh->mVertices[v].y,mesh->mVertices[v].z);
                box.UpdateMinMax(pos);
                radiusSq = Math::Max(radiusSq, pos.LengthSq());
            }
            meshInfo["aabb_min"] = {box.mMin.x, box.mMin.y, box.mMin.z};
            meshInfo["aabb_max"] = {box.mMax.x, box.mMax.y, box.mMax.z};
            meshInfo["bounding_radius"] = Math::Sqrt(radiusSq);

            string meshBinName = fbxPath.stem().string() + "_mesh" + std::to_string(i) + ".meshbin";
            meshInfo["binary_path"] = meshBinName;


            meshsJson.push_back(meshInfo);

            if (mesh->HasBones())
            {
                hasBones = true;
            }
        }
    }
    //スケルトンバイナリフラグを設定
    if (hasBones)
    {
        metaJson["import_settings"]["import_skeleton"] = true;
    }
    metaJson["cached_data"]["meshes"] = meshsJson;

    nlohmann::json materialsJson = nlohmann::json::array();
    nlohmann::json materialSlotsJson = nlohmann::json::array();
    if (scene && scene->HasMaterials())
    {
        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
        {
            aiMaterial* mat = scene->mMaterials[i];
            nlohmann::json matInfo;
            nlohmann::json slotInfo;

            //マテリアル名の取得
            matInfo["name"] = mat->GetName().C_Str();
            //ディフューズ(アルベド)テクスチャのパスを取得
            aiString texPath;
            if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
            {
                matInfo["albedo_map"] = texPath.C_Str();
            }
            else
            {
                matInfo["albedo_map"] = "";
            }

            aiColor4D diffuseColor;
            if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor))
            {
                matInfo["diffuse_color"] = 
                {
                 diffuseColor.r,
                 diffuseColor.g,
                 diffuseColor.b,
                 diffuseColor.a
                };
            }

            materialsJson.push_back(matInfo);

            slotInfo["slot_index"] = i;
            slotInfo["name"]       = mat->GetName().C_Str();

            //すでに.metaが存在し、assigned_materialが設定されている場合は上書きしない
            if (!isNewFile && metaJson.contains("material_slots") && metaJson["material_slots"].size() > i)
            {
                slotInfo["assigned_material"] = metaJson["material_slots"][i].value("assigned_material","");
            }
            else
            {
                slotInfo["assigned_material"] = "";
            }
            materialSlotsJson.push_back(slotInfo);
        }
    }
    metaJson["cached_data"]["materials"] = materialsJson;
    metaJson["material_slots"]           = materialSlotsJson;

    //ノード階層(ヒエラルキー)と初期トランスフォームの記録
    auto ParseNodeHierarchy = [&](auto& self, aiNode* node) -> nlohmann::json
    {
        nlohmann::json nodeJson;
        nodeJson["name"] = node->mName.C_Str();

        //ローカルトランスフォームの分解
        aiVector3D pos, scale;
        aiQuaternion rot;
        node->mTransformation.Decompose(scale, rot, pos);

        nodeJson["translation"] = {pos.x, pos.y, pos.z};
        nodeJson["rotation"]    = {rot.x, rot.y, rot.z, rot.w};
        nodeJson["scale"]       = {scale.x, scale.y, scale.z};

        //このノードに紐ずくメッシュのインデックス
        if (node->mNumMeshes > 0)
        {
            nodeJson["mesh_indices"] = nlohmann::json::array();
            for (unsigned int i = 0; i < node->mNumMeshes; i++)
            {
                unsigned int meshIndex = node->mMeshes[i];
                if (meshIndex < meshLocalIDs.size())
                {
                    nodeJson["mesh_indices"].push_back(meshLocalIDs[meshIndex]);
                }
            }
        }
        //子ノードの再帰処理
        if (node->mNumChildren > 0)
        {
            nodeJson["children"] = nlohmann::json::array();
            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                nodeJson["children"].push_back(self(self, node->mChildren[i]));
            }
        }
        return nodeJson;
    };

    if (scene && scene->mRootNode)
    {
        metaJson["cached_data"]["hierarchy"] = ParseNodeHierarchy(ParseNodeHierarchy,scene->mRootNode);
    }

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
    AssetMetaData metaData;
    if (metaJson.contains("guid"))
    {
        metaData.sGUID = metaJson["guid"].get<string>();
    }
    metaData.sSubMeshs = AssetDataBase::GetInstance().GetSubMeshPayload(fbxPath);
    //キャッシュデータの保存
    AssetDataBase::GetInstance().UpdateAssetData(fbxPath, metaData);
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
    header.sLayoutType  = (layout == VertexArray::PosNormTex) ? 0 : 1;
    header.sVertexCount = static_cast<uint32_t>(vertices.size());
    header.sIndexCount  = static_cast<uint32_t>(indices.size());
    header.sMin            = box.mMin;
    header.sMax            = box.mMax;
    header.sColliderRadius = radius; // 半径計算済みと仮定

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
        strncpy_s(bin.sName, b.name.c_str(),64);
        bin.sParentIndex = b.parentIndex;
        bin.sPosition    = b.position;
        bin.sRotation    = b.rotation;
        bin.sScale       = b.scale;

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
            bindTransform.sPosition = Vector3(pos.x, pos.y, pos.z);
            bindTransform.sRotation = Quaternion(rot.x, rot.y, rot.z, rot.w);
            bindTransform.sScale    = Vector3(scale.x, scale.y, scale.z);

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

            tracks[boneIndex][f].sPosition = Vector3(pos.x, pos.y, pos.z);
            tracks[boneIndex][f].sRotation = Quaternion(rot.x, rot.y, rot.z, rot.w);
            tracks[boneIndex][f].sScale    = Vector3(scale.x, scale.y, scale.z);
        }
    }

    std::ofstream out(animBinPath, std::ios::binary);
    if (!out)
    {
        Debug::Log("Failed to open animation bin for writing: %s",animBinPath.c_str());
        return;
    }

    AnimationBinHeader header;
    header.sDuration  = duration;
    header.sNumFrames = static_cast<uint32_t>(numFrames);
    header.sNumBones  = static_cast<uint32_t>(numBones);
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
    fs::path customPath = AssetDataBase::GetInstance().GeneratedMetaFilePath(fbxPath);

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

vector<string> AssetImporter::GetSubMeshNames(const fs::path& fbxPath)
{
    vector<string> meshNames;
    //FBXパスから対応する.metaファイルのパスを取得
    fs::path customPath = AssetDataBase::GetInstance().GeneratedMetaFilePath(fbxPath);
    //.metaファイルが存在しない場合は空のリストを返す
    if (!fs::exists(customPath))
    {
        return meshNames;
    }
    //.metaファイルを読み込む
    std::ifstream inFile(customPath);
    if (!inFile.is_open())
    {
        return meshNames;
    }

    nlohmann::json metaJson;
    inFile >> metaJson;
    inFile.close();

    if (metaJson.contains("cached_data") && metaJson["cached_data"].contains("meshes"))
    {
        const auto& meshs = metaJson["cached_data"]["meshes"];
        for (const auto& mesh : meshs)
        {
            if (mesh.contains("name"))
            {
                meshNames.push_back(mesh["name"].get<string>());
            }
        }
    }

    return meshNames;
}

vector<string> AssetImporter::GetSubMeshLocalID(const fs::path& fbxPath)
{
    vector<string> meshLocalIDs;
    // FBXパスから対応する.metaファイルのパスを取得
    fs::path customPath = AssetDataBase::GetInstance().GeneratedMetaFilePath(fbxPath);
    //.metaファイルが存在しない場合は空のリストを返す
    if (!fs::exists(customPath))
    {
        return meshLocalIDs;
    }
    //.metaファイルを読み込む
    std::ifstream inFile(customPath);
    if (!inFile.is_open())
    {
        return meshLocalIDs;
    }

    nlohmann::json metaJson;
    inFile >> metaJson;
    inFile.close();

    if (metaJson.contains("cached_data") &&
        metaJson["cached_data"].contains("meshes"))
    {
        const auto& meshs = metaJson["cached_data"]["meshes"];
        for (const auto& mesh : meshs)
        {
            if (mesh.contains("localID"))
            {
                meshLocalIDs.push_back(mesh["localID"].get<string>());
            }
        }
    }

    return meshLocalIDs;
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