#include "Mesh.h"
#include "FilePath.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"

Mesh::Mesh() {}

Mesh::~Mesh() {}

/*
bool Mesh::Load(const string& fileName, Renderer* renderer, int index)
{
    // ファイルの拡張子を取得
    string extension = fileName.substr(fileName.find_last_of('.') + 1);

    // **FBX の場合**
    if (extension == "fbx")
    {
        return LoadFromFBX(fileName, renderer, index);
    }

    return false;
}
*/

int Mesh::CheckMeshIndex(const string& fileName, Renderer* renderer)
{
    int           index = 0;
    std::ifstream fileCheck(fileName);
    if (!fileCheck)
    {
        SDL_Log("FBX file not found: %s", fileName.c_str());
        return false;
    }

    Assimp::Importer importer;
    const aiScene*   scene =
        importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs |
                                        aiProcess_GenNormals);

    if (!scene || !scene->HasMeshes())
    {
        SDL_Log("Assimp Error: %s", importer.GetErrorString());
        return false;
    }

    index = scene->mNumMeshes;

    return index;
}

bool Mesh::LoadFromMeshBin(const string& fileName, Renderer* renderer,
                           int index)
{
    string path = Sco::ExtensionFileName(fileName);
    string name = Sco::RemoveExtension(path);

    // 1:バイナリ情報に変換した頂点、インデックスデータをbinファイルから取得
    string number = std::to_string(index);
    // fileNameからPath部分だけ取り除く
    string result = Sco::RemoveString(name, File_P::ModelPath);

    string binaryFilePath =
        File_P::BinaryFilePath + result + number + File_P::BinaryPath;
    // バイナリファイルの確認
    std::ifstream in(binaryFilePath, std::ios::binary);
    if (!in)
    {
        SDL_Log("Failed to open mesh binary: %s", binaryFilePath.c_str());
        return false;
    }

    // バイナリデータの構造体宣言
    MeshBinHeader header;
    // 宣言した構造体に読み込んだファイルの情報を読み込む
    in.read((char*)&header, sizeof(header));
    // Textureのタイプを代入
    VertexArray::Layout layout = (header.layoutType == 0)
                                     ? VertexArray::PosNormTex
                                     : VertexArray::PosNormSkinTex;
    // 頂点とインデックスの数を計算
    mVertices.resize(header.vertexCount);
    mIndices.resize(header.indexCount);

    in.read((char*)mVertices.data(), sizeof(Vertex) * mVertices.size());
    in.read((char*)mIndices.data(), sizeof(uint32_t) * mIndices.size());

    // 中心位置や半径を再利用したい場合
    AABB box = AABB(Vector3::Infinity, Vector3::NegInfinity);
    box.mMin = header.min;
    box.mMax = header.max;

    // AABBの中心とサイズからOBBを作る（回転なし）
    Vector3    center   = (box.mMin + box.mMax) * 0.5f;
    Vector3    extents  = (box.mMax - box.mMin) * 0.5f;
    Quaternion rotation = Quaternion::Identity; // 方向なし
    OBB        obbBox = OBB(Vector3::Zero, Quaternion::Identity, Vector3::Zero);
    obbBox            = OBB(center, rotation, extents);

    mBoxs.push_back(box); // AABB中心などに使える
    mOBBBoxs.push_back(obbBox);
    mRadiusArray.push_back(header.colliderRadius);

    VertexArray* va =
        new VertexArray(mVertices.data(), header.vertexCount, layout,
                        mIndices.data(), header.indexCount);
    mVertexArrays.push_back(va);

    // 2:Assimpを使ってファイルからテクスチャとマテリアル情報を取得
    string assimpFilePath = fileName;
    // ファイルチェック
    std::ifstream fileCheck(assimpFilePath);
    if (!fileCheck)
    {
        SDL_Log("FBX file not found: %s", assimpFilePath.c_str());
        return false;
    }
    // モデル情報取得
    Assimp::Importer importer;
    const aiScene*   scene = importer.ReadFile(
        assimpFilePath, aiProcess_Triangulate | aiProcess_FlipUVs |
                            aiProcess_GenNormals | aiProcess_GlobalScale |
                            aiProcess_MakeLeftHanded |
                            aiProcess_FlipWindingOrder);
    // MeshCheck
    if (!scene || !scene->HasMeshes())
    {
        SDL_Log("Assimp Error: %s", importer.GetErrorString());
        return false;
    }

    aiMesh* mesh = scene->mMeshes[index];

    // テクスチャとマテリアルの情報を取得
    // テクスチャとマテリアルの読み込み
    std::unordered_map<string, Texture*> loadedTextures;
    string                               texFile = "MaterialTexure.png";
    // メッシュに関連付けられたマテリアルを取得
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    aiString    texturePath;
    // ファイルにFBXがあるか
    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) ==
        AI_SUCCESS)
    {
        texFile = texturePath.C_Str();

        // 埋め込みテクスチャかどうかチェック
        if (texFile[0] == '.')
        {
            int embeddedIndex = std::atoi(texFile.c_str() + 1);
            if (embeddedIndex < scene->mNumTextures)
            {
                aiTexture* embeddedTex = scene->mTextures[embeddedIndex];

                Texture* newTex = new Texture();
                if (newTex->LoadFromAssimp(embeddedTex))
                {
                    loadedTextures[texFile] = newTex;
                }
                else
                {
                    newTex->Unload();
                    delete newTex;
                }
            }
        }
        // 通常の外部テクスチャ
        else
        {
            if (loadedTextures.find(texFile) == loadedTextures.end())
            {
                Texture* newTex = new Texture();
                if (newTex->Load(File_P::ModelTexturePath + texFile))
                {
                    loadedTextures[texFile] = newTex;
                }
                else
                {
                    newTex->Unload();
                    delete newTex;
                }
            }
        }

        if (loadedTextures.find(texFile) != loadedTextures.end())
        {
            mTextures.push_back(loadedTextures[texFile]);
        }
    }
    // ないならマテリアル用のテクスチャロード
    else
    {
        // マテリアル用のテクスチャ取得
        if (loadedTextures.find(texFile) == loadedTextures.end())
        {
            Texture* newTex = new Texture();
            if (newTex->Load(File_P::ModelTexturePath + texFile))
            {
                loadedTextures[texFile] = newTex;
            }
            else
            {
                newTex->Unload();
                delete newTex;
            }
        }

        if (loadedTextures.find(texFile) != loadedTextures.end())
        {
            mTextures.push_back(loadedTextures[texFile]);
        }
    }

    MaterialInfo info{Vector4(0, 0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0),
                      Vector3(0, 0, 0), 0};

    aiColor4D diffuseColor;
    if (AI_SUCCESS ==
        aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor))
    {

        info.Color = Vector4(diffuseColor.r, diffuseColor.g, diffuseColor.b,
                             diffuseColor.a);
    }

    // 拡散色（Diffuse Color）の取得
    aiColor3D diffuse(1.0f, 1.0f, 1.0f);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);

    // 環境光（Ambient Color）の取得
    aiColor3D ambient(0.2f, 0.2f, 0.2f);
    material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);

    // 鏡面反射（Specular Color）の取得
    aiColor3D specular(0.5f, 0.5f, 0.5f);
    material->Get(AI_MATKEY_COLOR_SPECULAR, specular);

    // シェーダーに値を送る（glUniform3f を使用）
    info.Ambient  = Vector3(ambient.r, ambient.g, ambient.b);
    info.Diffuse  = Vector3(diffuse.r, diffuse.g, diffuse.b);
    info.Specular = Vector3(specular.r, specular.g, specular.b);

    mMaterialInfo.push_back(info);

    float shininess = 0.0f;
    if (scene->HasMaterials())
    {

        if (AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, shininess))
        {
            // デフォルト値を設定
            shininess = 100.0f;
        }
        shininess = shininess / 128.0f;

        info.Shininess = shininess;
    }

    // 読み込み成功
    return true;
}

bool Mesh::LoadFromSubMesh(const string& fbxPath, const string& localID)
{
    //.metaファイル(JSON)を読み込む
    filesystem::path metaPath = fbxPath + ".meta";

    if (!filesystem::exists(metaPath))
    {
        return false;
    }

    std::ifstream    metaFile(metaPath);
    if (!metaFile.is_open())return false;
    nlohmann::json   metaJson;
    try
    {
        metaFile >> metaJson;
    }
    catch (...)
    {
        return false;
    }
    metaFile.close();
    //先に親パスを指定
    filesystem::path binPath = "Binary/mesh";
    //localIDに一致するメッシュの情報をJSONから探す
    int targetIndex = -1;
    
    if (!metaJson.contains("cached_data") || !metaJson["cached_data"].contains("meshes"))return false;
    
    auto meshes      = metaJson["cached_data"]["meshes"];
    
    for (int i = 0; i < meshes.size(); i++)
    {
        if (meshes[i]["localID"] == localID)
        {
            binPath     = binPath / meshes[i]["binary_path"].get<string>();
            targetIndex = i;
            break;
        }
    }

    if (targetIndex == -1 || (binPath.empty() && !filesystem::exists(binPath)))return false;

    //.meshbinファイルをバイナリとして読み込む
    std::ifstream in(binPath, std::ios::binary);
    if (!in)
    {
        SDL_Log("Failed to open mesh binary: %s", binPath.c_str());
        return false;
    }

    //ヘッダー情報の読み込み
    //  バイナリデータの構造体宣言
    MeshBinHeader header;
    // 宣言した構造体に読み込んだファイルの情報を読み込む
    in.read((char*)&header, sizeof(header));
    // Textureのタイプを代入
    VertexArray::Layout layout = (header.layoutType == 0)
                                     ? VertexArray::PosNormTex
                                     : VertexArray::PosNormSkinTex;
    // 頂点とインデックスの数を計算
    mVertices.resize(header.vertexCount);
    mIndices.resize(header.indexCount);

    in.read((char*)mVertices.data(), sizeof(Vertex) * mVertices.size());
    in.read((char*)mIndices.data(), sizeof(uint32_t) * mIndices.size());
    in.close();

    // 中心位置や半径を再利用したい場合
    AABB box = AABB(Vector3::Infinity, Vector3::NegInfinity);
    box.mMin = header.min;
    box.mMax = header.max;

    // AABBの中心とサイズからOBBを作る（回転なし）
    Vector3    center   = (box.mMin + box.mMax) * 0.5f;
    Vector3    extents  = (box.mMax - box.mMin) * 0.5f;
    Quaternion rotation = Quaternion::Identity; // 方向なし
    OBB        obbBox = OBB(Vector3::Zero, Quaternion::Identity, Vector3::Zero);
    obbBox            = OBB(center, rotation, extents);

    mBoxs.push_back(box); // AABB中心などに使える
    mOBBBoxs.push_back(obbBox);
    mRadiusArray.push_back(header.colliderRadius);

    VertexArray* va =
        new VertexArray(mVertices.data(), header.vertexCount, layout,
                        mIndices.data(), header.indexCount);
    mVertexArrays.push_back(va);


    MaterialInfo info{Vector4(0, 0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0),
                      Vector3(0, 0, 0), 0};
    if (metaJson.contains("material_slots"))
    {
        int materialIndex = meshes[targetIndex].value("material_index", 0);

        if (materialIndex < metaJson["material_slots"].size())
        {
            auto slot = metaJson["material_slots"][materialIndex];
            string assignedMatPath = slot.value("assigned_material", "");

            if (assignedMatPath.empty())
            {
                //.meta内のcached_dataから直接マテリアル情報を生成する
                auto cachedMat = metaJson["cached_data"]["materials"][materialIndex];

                //色の読み込み
                if (cachedMat.contains("diffuse_color"))
                {
                    auto color = cachedMat["diffuse_color"];
                    info.Color = Vector4(color[0], color[1], color[2], color[3]);
                }
                else
                {
                    info.Color = Vector4(1, 1, 1, 1);
                }

                //一時的に設定
                //  拡散色（Diffuse Color）の取得
                aiColor3D diffuse(1.0f, 1.0f, 1.0f);
                // 環境光（Ambient Color）の取得
                aiColor3D ambient(0.2f, 0.2f, 0.2f);
                // 鏡面反射（Specular Color）の取得
                aiColor3D specular(0.5f, 0.5f, 0.5f);
                // シェーダーに値を送る（glUniform3f を使用）
                info.Ambient    = Vector3(ambient.r, ambient.g, ambient.b);
                info.Diffuse    = Vector3(diffuse.r, diffuse.g, diffuse.b);
                info.Specular   = Vector3(specular.r, specular.g, specular.b);
                /*
                float shininess = 0.0f;
                // デフォルト値を設定
                shininess = 50.0f;
                shininess = shininess / 128.0f;
                info.Shininess = shininess;
                */

                //テクスチャの読み込み
                string texMap = cachedMat.value("albedo_map", "");
                if (!texMap.empty())
                {
                    Texture* newTex = new Texture();
                    newTex->Load(File_P::ModelTexturePath + texMap);
                    mTextures.push_back(newTex);
                }
            }
            else
            {
                //マテリアル抽出済み状態(後々設計)
            }
        }
    }
    else
    {
        // マテリアルがないor取得出来なかった時の初期化マテリアル
        aiColor4D diffuseColor(0.5f, 0.5f, 0.5f, 1.0f);
        info.Color = Vector4(diffuseColor.r, diffuseColor.g, diffuseColor.b,
                             diffuseColor.a);
        // 拡散色（Diffuse Color）の取得
        aiColor3D diffuse(1.0f, 1.0f, 1.0f);
        // 環境光（Ambient Color）の取得
        aiColor3D ambient(0.2f, 0.2f, 0.2f);
        // 鏡面反射（Specular Color）の取得
        aiColor3D specular(0.5f, 0.5f, 0.5f);
        // シェーダーに値を送る（glUniform3f を使用）
        info.Ambient    = Vector3(ambient.r, ambient.g, ambient.b);
        info.Diffuse    = Vector3(diffuse.r, diffuse.g, diffuse.b);
        info.Specular   = Vector3(specular.r, specular.g, specular.b);
        /*
        float shininess = 0.0f;
        // デフォルト値を設定
        shininess      = 100.0f;
        shininess      = shininess / 128.0f;
        info.Shininess = shininess;
        */
    }
    mMaterialInfo.push_back(info);

    return true;
}

// FBX用読み込み関数
bool Mesh::LoadFromFBX(const string& fileName, Renderer* renderer, int index)
{
    // ファイルチェック
    std::ifstream fileCheck(fileName);
    if (!fileCheck)
    {
        SDL_Log("FBX file not found: %s", fileName.c_str());
        return false;
    }
    // モデル情報取得
    Assimp::Importer importer;
    const aiScene*   scene = importer.ReadFile(
        fileName, aiProcess_Triangulate | aiProcess_FlipUVs |
                      aiProcess_GenNormals | aiProcess_GlobalScale |
                      aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder);
    // MeshCheck
    if (!scene || !scene->HasMeshes())
    {
        SDL_Log("Assimp Error: %s", importer.GetErrorString());
        return false;
    }

    // メッシュをロード
    aiMesh* mesh;
    mVertices.clear();
    mIndices.clear();
    float radius = 0.0f;
    AABB  box    = AABB(Vector3::Infinity, Vector3::NegInfinity);
    // OBB obbBox = OBB(Vector3::Zero,Quaternion::Identity,Vector3::Zero);

    mesh = scene->mMeshes[index];

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
        /*
        // --- ボーンウェイト処理（VertexArray の期待順に合わせて必ず4スロット分
        push する） --- if (mesh->HasBones())
        {
            struct TmpWeight { int bone; float weight; };
            std::vector<TmpWeight> tmp;
            tmp.reserve(mesh->mNumBones);

            // 全ウェイトを収集
            for (unsigned int b = 0; b < mesh->mNumBones; ++b)
            {
                aiBone* bone = mesh->mBones[b];
                for (unsigned int w = 0; w < bone->mNumWeights; ++w)
                {
                    if (bone->mWeights[w].mVertexId == i &&
        bone->mWeights[w].mWeight > 0.0f)
                    {
                        tmp.push_back({ (int)b, bone->mWeights[w].mWeight });
                    }
                }
            }

            // 上位4つを選ぶ（存在しない分は無視）
            int pick = std::min<size_t>(4, tmp.size());
            if (!tmp.empty())
            {
                std::partial_sort(tmp.begin(), tmp.begin() + pick, tmp.end(),
                    [](const TmpWeight& a, const TmpWeight& b) { return a.weight
        > b.weight; });
            }

            // 上位4つの合計だけで正規化する
            float totalTop = 0.0f;
            for (int k = 0; k < pick; ++k) totalTop += tmp[k].weight;
            float invTotal = (totalTop > 0.0f) ? (1.0f / totalTop) : 0.0f;

            // 1) ボーンインデックス（uint8 x4）を一つの Vertex として push
            for (int k = 0; k < 4; ++k)
            {
                if (k < pick)
                    v.boneIDs[k] = static_cast<uint8_t>(tmp[k].bone); // cast
        明示 else v.boneIDs[k] = 0;
            }

            // 2) ボーンウェイト（float x4）をそれぞれ Vertex として
        push（順序は上位順） for (int k = 0; k < 4; ++k)
            {
                float wight;
                wight = (k < pick) ? (tmp[k].weight * invTotal) : 0.0f; //
        上位4つだけで正規化
                //mVertices.push_back(wight);
            }
        }
        */

        if (mesh->HasTextureCoords(0))
        {
            v.uv.x = uv.x;
            v.uv.y = uv.y;
        }

        mVertices.push_back(v);
    }

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
            mIndices.emplace_back(face.mIndices[0]);
            mIndices.emplace_back(face.mIndices[1]);
            mIndices.emplace_back(face.mIndices[2]);
        }
    }

    radius = Math::Sqrt(radius);
    // テクスチャとマテリアルの読み込み
    std::unordered_map<string, Texture*> loadedTextures;
    string                               texFile = "MaterialTexure.png";
    // メッシュに関連付けられたマテリアルを取得
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    aiString    texturePath;
    // ファイルにFBXがあるか
    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) ==
        AI_SUCCESS)
    {
        texFile = texturePath.C_Str();

        // 埋め込みテクスチャかどうかチェック
        if (texFile[0] == '.')
        {
            int embeddedIndex = std::atoi(texFile.c_str() + 1);
            if (embeddedIndex < scene->mNumTextures)
            {
                aiTexture* embeddedTex = scene->mTextures[embeddedIndex];

                Texture* newTex = new Texture();
                if (newTex->LoadFromAssimp(embeddedTex))
                {
                    loadedTextures[texFile] = newTex;
                }
                else
                {
                    newTex->Unload();
                    delete newTex;
                }
            }
        }
        // 通常の外部テクスチャ
        else
        {
            if (loadedTextures.find(texFile) == loadedTextures.end())
            {
                Texture* newTex = new Texture();
                if (newTex->Load(File_P::ModelTexturePath + texFile))
                {
                    loadedTextures[texFile] = newTex;
                }
                else
                {
                    newTex->Unload();
                    delete newTex;
                }
            }
        }

        if (loadedTextures.find(texFile) != loadedTextures.end())
        {
            mTextures.push_back(loadedTextures[texFile]);
        }
    }
    // ないならマテリアル用のテクスチャロード
    else
    {
        // マテリアル用のテクスチャ取得
        if (loadedTextures.find(texFile) == loadedTextures.end())
        {
            Texture* newTex = new Texture();
            if (newTex->Load(File_P::ModelTexturePath + texFile))
            {
                loadedTextures[texFile] = newTex;
            }
            else
            {
                newTex->Unload();
                delete newTex;
            }
        }

        if (loadedTextures.find(texFile) != loadedTextures.end())
        {
            mTextures.push_back(loadedTextures[texFile]);
        }
    }

    MaterialInfo info{Vector4(0, 0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0),
                      Vector3(0, 0, 0), 0};

    aiColor4D diffuseColor;
    if (AI_SUCCESS ==
        aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor))
    {

        info.Color = Vector4(diffuseColor.r, diffuseColor.g, diffuseColor.b,
                             diffuseColor.a);
    }

    // 拡散色（Diffuse Color）の取得
    aiColor3D diffuse(1.0f, 1.0f, 1.0f);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);

    // 環境光（Ambient Color）の取得
    aiColor3D ambient(0.2f, 0.2f, 0.2f);
    material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);

    // 鏡面反射（Specular Color）の取得
    aiColor3D specular(0.5f, 0.5f, 0.5f);
    material->Get(AI_MATKEY_COLOR_SPECULAR, specular);

    // シェーダーに値を送る（glUniform3f を使用）
    info.Ambient  = Vector3(ambient.r, ambient.g, ambient.b);
    info.Diffuse  = Vector3(diffuse.r, diffuse.g, diffuse.b);
    info.Specular = Vector3(specular.r, specular.g, specular.b);

    mMaterialInfo.push_back(info);

    float shininess = 0.0f;
    if (scene->HasMaterials())
    {

        if (AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, shininess))
        {
            // デフォルト値を設定
            shininess = 100.0f;
        }
        shininess = shininess / 128.0f;

        info.Shininess = shininess;
    }

    // Skinの場合のLayout変更
    VertexArray::Layout layout = VertexArray::PosNormTex;
    unsigned int        vertexCount =
        static_cast<unsigned>(mVertices.size()) / MeshLayout::MESH_VERTEXCOUNT;
    if (mesh->HasBones())
    {
        layout      = VertexArray::PosNormSkinTex;
        vertexCount = static_cast<unsigned>(mVertices.size()) /
                      MeshLayout::SKINMESH_VERTEXCOUNT;
    }

    mRadiusArray.push_back(radius);
    mBoxs.push_back(box);
    mOBBBoxs.push_back(obbBox);

    VertexArray* va =
        new VertexArray(mVertices.data(), (unsigned int)mVertices.size(),
                        layout, mIndices.data(), (unsigned int)mIndices.size());
    // 頂点配列の作成
    mVertexArrays.push_back(va);

    // fileNameからPath部分だけ取り除く
    string result = Sco::RemoveString(fileName, File_P::ModelPath);

    // バイナリに変換
    MeshBinHeader header;
    header.layoutType  = (layout == VertexArray::PosNormTex) ? 0 : 1;
    header.vertexCount = static_cast<uint32_t>(mVertices.size());
    header.indexCount  = static_cast<uint32_t>(mIndices.size());

    header.min            = box.mMin;
    header.max            = box.mMax;
    header.colliderRadius = radius; // 半径計算済みと仮定

    result               = Sco::ExtensionFileName(result);
    result               = Sco::RemoveExtension(result);
    string        number = std::to_string(index);
    std::ofstream out(File_P::BinaryFilePath + result + number +
                          File_P::BinaryPath,
                      std::ios::binary);
    out.write((char*)&header, sizeof(header));
    out.write((char*)mVertices.data(), sizeof(Vertex) * mVertices.size());
    out.write((char*)mIndices.data(), sizeof(uint32_t) * mIndices.size());

    // 読み込み成功
    return true;
}

void Mesh::Unload()
{
    // テクスチャの解放
    for (int i = 0; i < mTextures.size(); i++)
    {
        if (mTextures[i] != nullptr)
        {
            mTextures[i]->Unload();
            delete mTextures[i];
            mTextures[i] = nullptr;
        }
    }
    mTextures.clear();
    for (int i = 0; i < mVertexArrays.size(); i++)
    {
        if (mVertexArrays[i] != nullptr)
        {
            delete mVertexArrays[i];
            mVertexArrays[i] = nullptr;
        }
    }
    mVertexArrays.clear();
    mVertices.clear();
    mIndices.clear();
    mBoxs.clear();
    mOBBBoxs.clear();
    mRadiusArray.clear();
    mMaterialInfo.clear();
    mShaderName.clear();
}

Texture* Mesh::GetTexture(size_t index)
{
    if (index < mTextures.size())
    {
        return mTextures[index];
    }
    else
    {
        return nullptr;
    }
}

Sphere Mesh::GetAABBFromSphere()
{
    float       radius    = mRadiusArray[0];
    const AABB& localAABB = mBoxs[0];
    Vector3     center    = (localAABB.mMin + localAABB.mMax) / 2.0f;
    return Sphere(center, radius);
}

Capsule Mesh::GetAABBFromCapsule()
{
    const AABB& localAABB = mBoxs[0];
    Vector3     minP      = localAABB.mMin;
    Vector3     maxP      = localAABB.mMax;

    // 半径を決定
    float half_x = (maxP.x - minP.x) / 2.0f;
    float half_z = (maxP.z - minP.z) / 2.0f;

    // Capsuleの半径は、軸以外の断面の最大の半長とする
    float capsuleRadius = std::max(half_x, half_z);

    // 線分の開始点と終了点 (Y軸をCapsuleの中心線とする)
    Vector3 start = Vector3(minP.x, minP.y + capsuleRadius, minP.z);
    Vector3 end   = Vector3(maxP.x, maxP.y - capsuleRadius, maxP.z);

    // X, Z軸はAABBの中心点に固定
    start.x = end.x = (minP.x + maxP.x) / 2.0f;
    start.z = end.z = (minP.z + maxP.z) / 2.0f;

    // 線分の長さが 2 * radius よりも短い場合は、線分長を 0
    // にして点（球）とする。
    if (start.y > end.y)
    {
        start.y = end.y = (minP.y + maxP.y) / 2.0f;
    }
    LineSegment segment(start, end);

    return Capsule(segment, capsuleRadius);
}
