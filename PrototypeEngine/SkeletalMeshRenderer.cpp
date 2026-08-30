#include "SkeletalMeshRenderer.h"
#include "BoneActor.h"
#include "EngineWindow.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "MaterialManager.h"
#include "AssetDataBase.h"

SkeletalMeshRenderer::SkeletalMeshRenderer(Entity* owner)
    : MeshRenderer(owner, true)
    , mSkeleton(nullptr)
    , mAnimator(nullptr)
    , mBounds()
    , mRootBone(nullptr)
{
    mName = "SkeletalMeshRenderer";

    mHeaderColor        = Vector4(0.6f, 0.6f, 1.0f, 1.0f);
    mHeaderHoveredColor = Vector4(0.5f, 0.5f, 0.8f, 1.0f);
    mHeaderActiveColor  = Vector4(0.6f, 0.6f, 1.0f, 1.0f);
}

SkeletalMeshRenderer::~SkeletalMeshRenderer() {}

bool SkeletalMeshRenderer::Draw(Shader* shader)
{
    if (mMeshs.size() <= 0)
        return false;
    for (unsigned int i = 0; i < mMeshs.size(); i++)
    {
        for (unsigned int j = 0; j < mMeshs[i]->GetVertexArrays().size(); j++)
        {
            if (mMeshs[i])
            {
                // Set the world transform
                shader->SetMatrixUniform(
                    "uWorldTransform",
                    mActor->GetTransform()->GetWorldTransform());
                if (mAnimator)
                {
                    // Set the matrix palette
                    shader->SetMatrixUniforms(
                        "uMatrixPalette", &mAnimator->GetPalette().mEntry[0],
                        SkeletonLayout::MAX_SKELETON_BONES);
                }
                Texture* t = nullptr;
                // Set the active texture
                t = mMeshs[i]->GetTexture(j);
                if (t)
                {
                    t->SetActive();
                }
                else
                {
                    shader->SetNoTexture();
                }
                MaterialInfo m = mMeshs[i]->GetMaterialInfo()[j];

                // 不透明度によってブレンド設定（1回だけで済むならループの外でもOK）
                if (m.Color.w < 1.0f)
                {
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glDepthMask(GL_FALSE); // 透明物体は深度書き込み無効（任意）
                }
                else
                {
                    glDisable(GL_BLEND);
                    glDepthMask(GL_TRUE); // 不透明物体は通常通り
                }
                // マテリアルの色を設定
                shader->SetColorUniform(m);
                // メッシュの頂点配列をアクティブに設定します
                VertexArray* va = mMeshs[i]->GetVertexArrays()[j];
                va->SetActive();
                // 描画
                glDrawElements(GL_TRIANGLES, va->GetNumIndices(),
                               GL_UNSIGNED_INT, nullptr);
            }
        }
    }
    return true;
}

void SkeletalMeshRenderer::DrawForShadowMap(Shader* shader)
{
    for (unsigned int i = 0; i < mMeshs.size(); i++)
    {
        for (unsigned int j = 0; j < mMeshs[i]->GetVertexArrays().size(); j++)
        {
            if (mMeshs[i])
            {
                // Set the world transform
                shader->SetMatrixUniform(
                    "uWorldTransform",
                    mActor->GetTransform()->GetWorldTransform());
                if (mAnimator)
                {
                    // Set the matrix palette
                    shader->SetMatrixUniforms(
                        "uMatrixPalette", &mAnimator->GetPalette().mEntry[0],
                        SkeletonLayout::MAX_SKELETON_BONES);
                }

                // ブレンドなどはシャドウマップ描画時は一切不要
                glDisable(GL_BLEND);
                glDepthMask(GL_TRUE);

                // メッシュの頂点配列をアクティブに設定します
                VertexArray* va = mMeshs[i]->GetVertexArrays()[j];
                va->SetActive();
                // 描画
                glDrawElements(GL_TRIANGLES, va->GetNumIndices(),
                               GL_UNSIGNED_INT, nullptr);
            }
        }
    }
}

void SkeletalMeshRenderer::Update(float deltaTime)
{
    /*
    if (!mAnimator) { return; }
    mAnimator->Update(deltaTime);
    */
}
void SkeletalMeshRenderer::LoadSkeletonMesh(const string& fileName,
                                            ActorObject*  actor)
{
    /*
    const vector<class Mesh*>& mesh =
        EngineWindow::GetRenderer()->GetMeshs(fileName);
    mMeshs.insert(mMeshs.end(), mesh.begin(), mesh.end());
    */

    Skeleton* sk = mGame->GetSkeleton(fileName);
    mSkeleton    = sk;
    if (mSkeleton != nullptr)
    {
        mSkeleton->SetParentActor(actor);
    }
    mIsSkeletal = true;
}

void SkeletalMeshRenderer::SetSkeleton(Skeleton* sk, ActorObject* actor)
{
    mSkeleton = nullptr;
    mSkeleton = sk;

    if (!mSkeleton)
    {
        return;
    }

    const auto& bones = mSkeleton->GetBones();

    // 1.mParentIndex を使って親子関係を構築
    for (size_t i = 0; i < bones.size(); i++)
    {
        const BoneActor*      bone       = bones[i];
        BoneActor*            childActor = mSkeleton->GetBoneActor()[i];

        // mParentIndexは親ボーンのインデックス
        if (bone->GetParentIndex() != -1)
        {
            // 親ボーンのActorを取得
            BoneActor* parentActor = mSkeleton->GetBoneActor()[bone->GetParentIndex()];

            // Transform::AddChildActor() を呼び出して親子関係を結ぶ
            // Transform.cpp に実装されている AddChildActor を利用
            parentActor->GetTransform()->AddChildActor(childActor);

            // (オプション) SkeletalMeshRendererのオーナーActorを親にする場合
            // childActor->SetParentActor(this->GetOwner());
        }
        else
        {
            // ルートボーンの場合
            // ルートアクターをSkeletonMeshRendererを持つオーナーのオブジェクトの子にする
            if (mOwner)
            {
                mActor->GetTransform()->AddChildActor(childActor);
            }
        }
        /*
        // ローカルバインドポーズを設定
        // BoneActorのTransformにボーンのローカル位置と回転を設定し、
        // ボーンオフセット（BindPose）を反映させます。
        Matrix4 bindPose = bone.mLocalBindPose;
        childActor->SetLocalPosition(bindPose.GetTranslation());
        childActor->SetLocalRotation(bindPose.GetRotation());
        childActor->SetLocalScale(bindPose.GetScale());
        */
    }
}

void SkeletalMeshRenderer::SetAnimator(Animator* animator)
{
    mAnimator = nullptr;
    mAnimator = animator;
}

void SkeletalMeshRenderer::Serialize(json& j) const
{
    // 1. ベースクラス (MeshRenderer) のシリアライズを呼び出す
    //    -> これにより、mMeshFilePath やその他の基本プロパティが書き込まれる
    MeshRenderer::Serialize(j);

    // 2. コンポーネントの型を「SkeletalMeshRenderer」で上書き
    //    -> ActorObject::Deserialize()のファクトリー処理で、この型を使って
    //       SkeletalMeshRendererのインスタンスが生成されるようにする
    // j["Type"] = "SkeletalMeshRenderer";

    // 3. (必要に応じて) スケルタルメッシュ固有のプロパティを追記
    //    mAnimatorは通常、Actorの別コンポーネントとしてシリアライズされるため、ここでは省略
}

void SkeletalMeshRenderer::Deserialize(const json& j)
{
    // 1. ベースクラスのデシリアライズを呼び出す (MeshRenderer::Deserialize)
    //    -> JSONから mMeshFilePath
    //    を読み込み、EngineWindow::GetRenderer()->GetMeshs() を呼び出して
    //       mMeshs (メッシュ) のロードと設定を完了させる。
    MeshRenderer::Deserialize(j);

    // 2. ベースクラスで読み込まれたファイルパス (mMeshFilePath)
    // を使ってスケルトンをロードする
    std::string fileName =
        GetMeshFilePath(); // GetMeshFilePath() が mMeshFilePath を返す前提

    // SkeletalMeshRenderer::LoadSkeletonMesh のスケルトンロード部分のロジック
    Skeleton* sk = mOwner->GetGame()->GetSkeleton(fileName);
    mSkeleton    = sk;
    /*
    if (mSkeleton != nullptr)
    {
        mSkeleton->SetParentActor(mOwner); // Actorにスケルトンを設定
    }
    */

    mIsSkeletal = true; // スケルトンを持っていることを明示

    // 3. アニメーターのリンク（Animatorが別コンポーネントの場合）
    //    ActorObjectにアタッチされているAnimatorコンポーネントを取得し、mAnimatorに設定します。
    //    (注: Animatorクラスがある前提)
    //    Animator* animator = mOwner->GetComponent<Animator>();
    //    SetAnimator(animator);
}

void SkeletalMeshRenderer::DrawCustomGUI(
    const std::vector<PropertyInfo>& properties)
{
    ImGui::PushID(this);

    // MeshRendererのプロパティ
    ImGui::Text("Properties");

    ImGui::NewLine();

    ImGui::Text("Bounds");
    ImGui::DragFloat3("Center", &mBounds.sCenter.x);
    ImGui::DragFloat3("Extent", &mBounds.sExtent.x);


    // 1.ファイルパスの取得
    filesystem::path currentPath = mFilePath;
    static char      pathBuffer[256];
    strncpy_s(pathBuffer, currentPath.filename().stem().string().c_str(),
              sizeof(pathBuffer));
    pathBuffer[sizeof(pathBuffer) - 1] = '\0';
    ImGui::Text("Mesh");
    // 2.ファイルパスの入力フィールド
    ImGui::InputText("Mesh File Path", pathBuffer, sizeof(pathBuffer),
                     ImGuiInputTextFlags_ReadOnly);

    // 3.ファイルロードボタン(ここでファイル選択UIを開くか、ProjectPanelからのDrag&Dropを想定)
    // Drag&Drop想定
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload("SUB_MESH_ITEM"))
        {
            // ペイロードがファイルパスであると仮定
            const SubMeshPayload* data = (const SubMeshPayload*)payload->Data;
            // ファイルパスを使いロード処理を呼び出す
            LoadFilePathAndID(data->sSubMeshName, data->sLocalID);
            mLocalID = data->sLocalID;
        }
        ImGui::EndDragDropTarget();
    }
    if (ImGui::Button("Clear Meshs"))
    {
        SetMeshs({});
        mFilePath = "";
        mAlpha    = 1.0f;
    }

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::Text("Materials");
    if (!mMeshs.empty() && mMeshs[0])
    {
        // メッシュが持つサブメッシュの数(=マテリアルスロット数)を取得
        int materialCount = mMeshs[0]->GetVertexArrays().size();

        // 配列のサイズをスロット数に合わせる
        if (mMaterials.size() != materialCount)
        {
            mMaterials.resize(materialCount, nullptr);
        }

        for (int i = 0; i < materialCount; i++)
        {
            ImGui::PushID(i);
            string matLabel = "Element" + std::to_string(i);

            // 割り当てられているか確認してパスを表示
            string displayPath = mMaterials[i] ? mMaterials[i]->GetFilePath()
                                               : "None (Mesh Default)";
            filesystem::path p(displayPath);
            displayPath = p.filename().string(); // ファイル名だけ表示

            char matBuffer[256];
            strncpy_s(matBuffer, displayPath.c_str(), sizeof(matBuffer));
            matBuffer[sizeof(matBuffer) - 1] = '\0';

            ImGui::InputText(matLabel.c_str(), matBuffer, sizeof(matBuffer),
                             ImGuiInputTextFlags_ReadOnly);

            // ドラッグ&ドロップで.matを割り当てる
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload =
                        ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                {
                    const char*      payloadPath = (const char*)payload->Data;
                    filesystem::path droppedPath(payloadPath);

                    if (droppedPath.extension() == ".mat")
                    {
                        mMaterials[i] =
                            MaterialManager::GetMaterial(droppedPath.string());
                    }
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::SameLine();
            if (ImGui::Button("*"))
            {
                mMaterials[i] = nullptr;
            }
            ImGui::PopID();
        }
    }

    ImGui::NewLine();

    ImGuiTableFlags tableFlags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp;

    if (ImGui::BeginTable("MeshRendererSettingsTable", 2, tableFlags))
    {
        ImGui::TableNextRow();

        if (!mMeshs.empty())
        {
            ImGuiHelper::TableSliderFloat("Alpha Setting", &mAlpha, 0.0f, 1.0f,
                                          "%.2f");

            if (mAlpha != mMeshs[0]->GetMaterialInfo()[0].Color.w)
            {
                SetMaterialAlpha(mAlpha);
            }
        }

        string shadowText = "Shadow";
        if (mShadowFrag)
        {
            shadowText += "/On";
        }
        else
        {
            shadowText += "/Off";
        }
        ImGuiHelper::TableCheckbox(shadowText.c_str(), &mShadowFrag);

        ImGui::EndTable();
    }

    ImGui::NewLine();

    // 1.RootBoneの取得処理
    Transform* currentBone = mRootBone;
    static char boneNameBuffer[256];
    string      currentBoneName = "";
    if (currentBone)
    {
        currentBoneName = currentBone->GetOwner() ? currentBone->GetOwner()->GetName() : "";
    }
    strncpy_s(boneNameBuffer, currentBoneName.c_str(),sizeof(boneNameBuffer));
    boneNameBuffer[sizeof(boneNameBuffer) - 1] = '\0';
    ImGui::Text("Root Bone");
    // 2.RootBoneのドラッグフィールド
    ImGui::InputText("No Root Bone", boneNameBuffer, sizeof(boneNameBuffer),
                     ImGuiInputTextFlags_ReadOnly);

    // RootBoneのドラッグ処理
    // Drag&Drop想定
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
        {
            // ペイロードがファイルパスであると仮定
            Transform* data = (Transform*)payload->Data;
            mRootBone = data;
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::Separator();

    ImGui::PopID();


    /*
    ImGui::PushID(this);

    // MeshRendererのプロパティ
    ImGui::Text("Properties");

    ImGui::NewLine();

    // 1.ファイルパスの取得
    string      currentPath = mFilePath;
    static char pathBuffer[256];
    strncpy_s(pathBuffer, currentPath.c_str(), sizeof(pathBuffer));
    pathBuffer[sizeof(pathBuffer) - 1] = '\0';

    // 2.ファイルパスの入力フィールド
    ImGui::InputText("Mesh File Path", pathBuffer, sizeof(pathBuffer),
                     ImGuiInputTextFlags_ReadOnly);

    // 3.ファイルロードボタン(ここでファイル選択UIを開くか、ProjectPanelからのDrag&Dropを想定)
    // Drag&Drop想定
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
        {
            // ペイロードがファイルパスであると仮定
            const char* dropPath = (const char*)payload->Data;
            string      path     = Sco::ExtensionFileName(dropPath);
            // ファイルパスを使いロード処理を呼び出す
            vector<class Mesh*> mesh;// =
                //EngineWindow::GetRenderer()->GetMeshs(path);
            SetMeshs(mesh);
            mFilePath = path;

            Skeleton* sk = mGame->GetSkeleton(path);
            mSkeleton    = sk;
            // ボーンの親子関係を構築
            if (mSkeleton != nullptr)
            {
                SetSkeleton(mSkeleton, mActor);
            }
            mIsSkeletal = true;
        }
        ImGui::EndDragDropTarget();
    }
    // ボタンクリックでファイル選択ダイアログを開く実装
    if (ImGui::Button("Load Mesh from File"))
    {
        // 外部のファイル選択ダイアログ (例: nativefiledialog) を開き、
        // 選択されたファイルパスを meshRenderer->Load(...) に渡す。
    }

    ImGui::Separator();
    */
}

Component* SkeletalMeshRenderer::Clone(Entity* newOwner) const
{

    return nullptr;
}
