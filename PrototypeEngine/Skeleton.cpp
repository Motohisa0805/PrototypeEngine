#include "Skeleton.h"
#include "BoneActor.h"
#include "DebugManager.h"
#include "FilePath.h"
#include "StringConvertOperation.h"
#include "AssetImporter.h"

SkeletonData::~SkeletonData() {}
// ファイル形式で読み込み関数を変更
bool SkeletonData::Load(const string& fileName)
{
    // ファイルの拡張子を取得
    string extension = fileName.substr(fileName.find_last_of('.') + 1);

    // **FBX の場合**
    if (extension == "fbx")
    {
        return LoadFromFBX(fileName);
    }

    return false;
}
// バイナリ限定の読み込み
bool SkeletonData::LoadFromSkeletonBin(const string& fileName)
{
    filesystem::path path(fileName);
    std::ifstream in((filesystem::path) "Binary/skeleton" / (path.stem().string() + ".skelbin"),
                     std::ios::binary);
    if (!in)
    {
        Debug::ErrorLog("Failed to open Skeleton bin: %s", fileName.c_str());
        return false;
    }

    uint32_t boneCount = 0;
    in.read((char*)&boneCount, sizeof(uint32_t));

    if (boneCount > SkeletonLayout::MAX_SKELETON_BONES)
    {
        Debug::ErrorLog("Skeleton bin exceeds max bones");
        return false;
    }

    mBones.clear();
    mBones.reserve(boneCount);

    for (uint32_t i = 0; i < boneCount; ++i)
    {
        BoneInfo bin{};
        if(!in.read((char*)&bin, sizeof(AssetImporter::SkeletonBinHeader)))continue;

        BoneInfo boneInfo{};
        strncpy_s(boneInfo.sName, bin.sName, sizeof(boneInfo.sName) - 1);
        boneInfo.sNodeHash    = bin.sNodeHash;
        boneInfo.sParentIndex = bin.sParentIndex;
        boneInfo.sLocalPos    = bin.sLocalPos;
        boneInfo.sLocalRot    = bin.sLocalRot;
        boneInfo.sLocalScale  = bin.sLocalScale;
        int boneIndex              = static_cast<int>(mBones.size());
        mBoneNameToIndex[bin.sName] = boneIndex;
        mBones.push_back(boneInfo);

        /*
        BoneActor* b = new BoneActor();
        b->SetBoneIndex(static_cast<int>(mBones.size()));
        b->SetBoneName(bin.name);
        b->SetParentIndex(bin.parentIndex);
        b->GetTransform()->SetPosition(bin.position);
        b->GetTransform()->SetRotation(bin.rotation);
        b->GetTransform()->SetScale(bin.scale);

        // boneNameToIndexにボーン名をキーにボーン番号を格納
        mBoneNameToIndex[b->GetName()] = static_cast<int>(mBoneActors.size());

        // ボーンベクターに格納
        mBoneActors.push_back(b);

        // assimpではオフセット行列をそのまま利用
        mBoneActors[i]->SetGlobalInvBindPose(b->GetGlobalInvBindPose());

        if (bin.parentIndex != -1 && bin.parentIndex < static_cast<int>(mBoneActors.size()))
        {
            BoneActor* parentBone = mBoneActors[bin.parentIndex];
            b->GetTransform()->AddParentActor(parentBone);
        }
        */
    }

    ComputeGlobalInvBindPose();
    return true;
}

bool SkeletonData::LoadFromFBX(const string& fileName)
{
    // ファイル読み込み
    Assimp::Importer importer;
    // 三角形でポリゴンを取得、ボーンのウェイトを最大4つに制限、スケーリングを1unitに
    const aiScene* scene = importer.ReadFile(
        fileName, aiProcess_Triangulate | aiProcess_LimitBoneWeights |
                      aiProcess_GlobalScale | aiProcess_MakeLeftHanded |
                      aiProcess_FlipUVs | aiProcess_FlipWindingOrder);
    // モデルがあるか確認
    if (!scene || !scene->HasMeshes())
    {
        // ないならエラーメッセージ
        Debug::ErrorLog("Failed to load FBX: %s", importer.GetErrorString());
        return false;
    }
    // ボーンの初期化
    mBones.clear();
    mBoneNameToIndex.clear();
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
            if (mBoneNameToIndex.find(boneName) != mBoneNameToIndex.end())
                continue;

            BoneInfo boneInfo{};
            strncpy_s(boneInfo.sName, boneName.c_str(), sizeof(boneInfo.sName) - 1);
            boneInfo.sParentIndex = -1;

            boneInfo.sInverseBindPose = Matrix4::ConvertToMatrix4(bone->mOffsetMatrix);

            int boneIndex = static_cast<int>(mBones.size());
            mBoneNameToIndex[boneName] = boneIndex;
            mBones.push_back(boneInfo);

            /*
            // ボーンの構造体
            BoneActor* b = new BoneActor();
            // ボーンのmOffsetMatrixをvectorに格納
            mOffsetMatrix.push_back(bone->mOffsetMatrix);

            b->SetBoneIndex(static_cast<int>(mBoneActors.size()));
            // ボーン本来の名前を代入
            b->SetBoneName(boneName);
            // 後で SetParentBones() で設定する
            b->SetParentIndex(-1);

            // バインドポーズの変換
            // ボーンのmOffsetMatrix取得
            aiMatrix4x4  bindPose = bone->mOffsetMatrix;
            aiVector3D   pos;
            aiQuaternion rot;
            aiVector3D   scale;
            // ボーンのバインドポーズを各値に分解
            bindPose.Decompose(scale, rot, pos);

            // ローカルのバインドポーズに回転、平行移動、スケーリングを格納
            b->GetTransform()->SetRotation(
                Quaternion(rot.x, rot.y, rot.z, rot.w));
            b->GetTransform()->SetPosition(Vector3(pos.x, pos.y, pos.z));
            b->GetTransform()->SetScale(
                Vector3(scale.x, scale.y, scale.z));
            // boneNameToIndexにボーン名をキーにボーン番号を格納
            mBoneNameToIndex[boneName] = static_cast<int>(mBoneActors.size());
            // 同じくmBoneTransformにボーンの番号を格納
            mBoneTransform[b->GetName()] =
                static_cast<int>(mBoneActors.size());
            // ボーンベクターに格納
            mBoneActors.push_back(b);

            // assimpではオフセット行列をそのまま利用
            mBoneActors[i]->SetGlobalInvBindPose(b->GetGlobalInvBindPose());
            */
        }
    }
    if (scene->mRootNode != nullptr)
    {
        // 親子関係を設定
        SetParentBones(scene->mRootNode, -1);
    }

    // fileNameからPath部分だけ取り除く
    string result = Sco::RemoveString(fileName, File_P::ModelPath);
    result        = Sco::RemoveExtension(result);
    std::ofstream out(File_P::BinaryFilePath + result + File_P::BinarySkelPath,
                      std::ios::binary);
    if (!out)
    {
        Debug::ErrorLog("Failed to open skelbin for writing.");
        return false;
    }

    uint32_t boneCount = static_cast<uint32_t>(mBones.size());
    out.write((char*)&boneCount, sizeof(uint32_t));

    for (BoneInfo b : mBones)
    {
        BoneInfo bin{};
        strncpy_s(bin.sName, b.sName, sizeof(bin.sName) - 1);
        //bin.sName        = b.sName;
        bin.sNodeHash   = AssetImporter::GenerateNameHash(b.sName);
        bin.sParentIndex = b.sParentIndex;
        bin.sLocalPos    = b.sLocalPos;
        bin.sLocalRot    = b.sLocalRot;
        bin.sLocalScale  = b.sLocalScale;

        out.write((char*)&bin, sizeof(AssetImporter::SkeletonBinHeader));
    }

    return true;
}

void SkeletonData::SetParentBones(aiNode* node, int parentIndex)
{
    // 不明なボーンの場合に次にそのまま再起するための処理を追加
    string nodeName  = node->mName.C_Str();
    int    currentIndex = parentIndex;

    // このノードがボーンとして登録されているか確認
    if (mBoneNameToIndex.find(nodeName) != mBoneNameToIndex.end())
    {
        currentIndex = mBoneNameToIndex[nodeName];

        BoneInfo& currentBone = mBones[currentIndex];

        currentBone.sParentIndex = parentIndex;

        aiVector3D pos;
        aiQuaternion rot;
        aiVector3D   scale;
        node->mTransformation.Decompose(scale, rot, pos);

        currentBone.sLocalPos = Vector3(pos.x, pos.y, pos.z);
        currentBone.sLocalRot = Quaternion(rot.x, rot.y, rot.z, rot.w);
        currentBone.sLocalScale = Vector3(scale.x, scale.y, scale.z);
    }

    // 子ノードを再帰的に処理
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        SetParentBones(node->mChildren[i], currentIndex);
    }
}

void SkeletonData::SetParentActor(ActorObject* parent) 
{
    /*
    if (!mBoneActors.empty())
    {
        mBoneActors[0]->GetTransform()->AddParentActor(parent);
    }
    */
}

ActorObject* SkeletonData::FindActorByName(ActorObject*  current,
                                                   const string& name)
{
    if (!current)
        return nullptr;
    if (current->GetName() == name)
        return current;

    for (auto* childTransform : current->GetTransform()->GetChildActorList())
    {
        ActorObject* found = FindActorByName(childTransform, name);
        if (found)
        {
            return found;
        }
    }
    return nullptr;
}

void SkeletonData::ComputeGlobalInvBindPose()
{
    if (mBones.empty())return;

    //ワールドバインドポーズ行列を一時的に保持する配列
    vector<Matrix4> globalBindPoses(mBones.size());

    //ルートボーンから順にワールド行列(バインドポーズ)を強制計算させる
    for (size_t i = 0; i < mBones.size(); ++i)
    {
        Matrix4 localScaleMat = Matrix4::CreateScale(mBones[i].sLocalScale);
        Matrix4 localRotMat = Matrix4::CreateFromQuaternion(mBones[i].sLocalRot);
        Matrix4 localTransMat = Matrix4::CreateTranslation(mBones[i].sLocalPos);

        Matrix4 localBindPose = localScaleMat * localRotMat * localTransMat;

        int parentIdx = mBones[i].sParentIndex;

        if (parentIdx != -1 && parentIdx < static_cast<int>(i))
        {
            globalBindPoses[i] = localBindPose * globalBindPoses[parentIdx];
        }
        else
        {
            globalBindPoses[i] = localBindPose;
        }
        Matrix4 invMat = globalBindPoses[i];
        invMat.Invert();
        mBones[i].sInverseBindPose = invMat;
        /*
        mBoneActors[i]->GetTransform()->SetDirty();
        mBoneActors[i]->GetTransform()->ComputeWorldTransform();

        // 計算されたワールド行列をグローバルバインドポーズとして保存
        Matrix4 bindPoseMat =
            mBoneActors[i]->GetTransform()->GetWorldTransform();
        bindPoseMat.Invert();
        mBoneActors[i]->SetGlobalInvBindPose(bindPoseMat);
        */
    }
    /*
    // Resize to number of bones, which automatically fills identity
    mGlobalInvBindPoses.resize(GetNumBones());

    // Step 1: Compute global bind pose for each bone

    // The global bind pose for root is just the local bind pose
    mGlobalInvBindPoses[0] = mBoneActors[0]->GetTransform()->GetLocalTransform();

    // Each remaining bone's global bind pose is its local pose
    // multiplied by the parent's global bind pose
    for (size_t i = 1; i < mGlobalInvBindPoses.size(); i++)
    {
        Matrix4 localMat = mBoneActors[i]->GetTransform()->GetLocalTransform();
        mGlobalInvBindPoses[i] =
            localMat * mGlobalInvBindPoses[mBoneActors[i]->GetParentIndex()];
    }

    // Step 2: Invert
    for (size_t i = 0; i < mGlobalInvBindPoses.size(); i++)
    {
        mGlobalInvBindPoses[i].Invert();
    }
    */
    int test = 0;
}