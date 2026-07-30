#include "MeshRenderer.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include "EngineWindow.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "MaterialManager.h"

MeshRenderer::MeshRenderer(Entity* owner, bool isSkeletal)
    : Component(owner)
    , mVisible(true)
    , mIsSkeletal(isSkeletal)
    , mFilePath("")
    , mAlpha(1.0f)
    , mShadowFrag(true)
{
    mName = "MeshRenderer";

    mHeaderColor        = Vector4(0.4f, 0.4f, 0.8f, 1.0f);
    mHeaderHoveredColor = Vector4(0.3f, 0.3f, 0.6f, 1.0f);
    mHeaderActiveColor  = Vector4(0.4f, 0.4f, 0.8f, 1.0f);

    EngineWindow::GetRenderer()->AddMeshComp(this);
}

MeshRenderer::~MeshRenderer()
{
    EngineWindow::GetRenderer()->RemoveMeshComp(this);

    mMaterials.clear();
}

bool MeshRenderer::Draw(Shader* shader)
{
    // メッシュがない場合は描画しない
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

                MaterialInfo m = mMeshs[i]->GetMaterialInfo()[j];
                Texture* t = mMeshs[i]->GetTexture(j);
                if (t == nullptr)
                {
                    t = Texture::GetWhiteTexture();
                }

                if (j < mMaterials.size() && mMaterials[j] != nullptr)
                {
                    MaterialData& md = mMaterials[j]->GetData();
                    m.Color          = md.sDiffuseColor;
                    m.Diffuse        = Vector3(md.sDiffuseColor.x, md.sDiffuseColor.y,md.sDiffuseColor.z);
                    m.Ambient        = md.sAmbientColor;
                    m.Specular       = md.sSpecularColor;
                    m.Shininess      = md.sShininess;
                    m.Metallic       = md.sMetallic;
                    m.Roughness      = md.sRoughness;
                    m.Emissive       = md.sEmissive;

                    //テクスチャがあれば上書き
                    if (mMaterials[j]->GetTexture() != nullptr)
                    {
                        t = mMaterials[j]->GetTexture();
                    }
                }
                
                
                if (t)
                {
                    t->SetActive();
                }
                else
                {
                    shader->SetNoTexture();
                }

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

void MeshRenderer::DrawForShadowMap(Shader* shader)
{
    // 影フラグがOFFなら描画しない
    if (!mShadowFrag)
    {
        return;
    }
    for (unsigned int i = 0; i < mMeshs.size(); i++)
    {
        for (unsigned int j = 0; j < mMeshs[i]->GetVertexArrays().size(); j++)
        {
            if (!mMeshs[i])
                continue;

            // ワールド変換のみ設定
            shader->SetMatrixUniform(
                "uWorldTransform", mActor->GetTransform()->GetWorldTransform());

            // ブレンドなどはシャドウマップ描画時は一切不要
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);

            // 頂点配列をアクティブに
            VertexArray* va = mMeshs[i]->GetVertexArrays()[j];
            va->SetActive();

            // 描画
            glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT,
                           nullptr);
        }
    }
}

void MeshRenderer::SetMaterialAlpha(float alpha)
{
    float a = Math::Clamp(alpha, 0.0f, 1.0f);
    if (!mMeshs.empty())
    {
        for (auto& mesh : mMeshs)
        {
            if (mesh == nullptr)
                continue;

            vector<MaterialInfo> info = mesh->GetMaterialInfo();
            for (int i = 0; i < info.size(); ++i)
            {
                info[i].Color = Vector4(info[i].Color.x, info[i].Color.y,
                                        info[i].Color.z, a);
            }
            mesh->SetMaterialInfo(info);
        }
    }
}

void MeshRenderer::SetIsRun(bool run)
{
    Component::SetIsRun(run);
    if (run)
    {
        EngineWindow::GetRenderer()->AddMeshComp(this);
    }
    else
    {
        EngineWindow::GetRenderer()->RemoveMeshComp(this);
    }
}

void MeshRenderer::Serialize(json& j) const
{
    Component::Serialize(j);
    // ロード元のファイルパスをそのままJSONに書き込む
    j["FilePath"] = mFilePath;
    j["LocalID"]  = mLocalID;

    //マテリアルパス
    json matPaths = json::array();
    for (size_t i = 0; i < mMaterials.size(); ++i)
    {
        if (mMaterials[i] != nullptr)
        {
            matPaths.push_back(mMaterials[i]->GetFilePath());
        }
        else
        {
            matPaths.push_back("");
        }
        j["MaterialPaths"] = matPaths;
    }

    // メッシュレンダラー固有の他のプロパティも追加
    j["Visible"]    = mVisible;
    j["IsSkeletal"] = mIsSkeletal;
    j["Alpha"]      = mAlpha;
    j["ShadowFrag"] = mShadowFrag;
}

void MeshRenderer::Deserialize(const json& j)
{
    Component::Deserialize(j);
    // モデルパスがあるなら
    if (j.contains("FilePath") && j.contains("LocalID"))
    {
        // 1. JSONからファイルパスを取得する
        std::string filePath = j.at("FilePath").get<std::string>();

        // 2. メンバ変数にファイルパスを設定
        mFilePath = filePath;
        mLocalID  = j.at("LocalID").get<std::string>();

        // 3. ファイルパスを使って、Rendererからメッシュをロードし、設定する
        //    元のコードにあった処理をここで実行します
        Mesh* mesh = EngineWindow::GetRenderer()->GetSubMesh(mFilePath,mLocalID);
        if (mesh)
        {
            SetMesh({mesh});
        }
    }

    //マテリアルの復元
    if (j.contains("MaterialPaths"))
    {
        auto matPaths = j["MaterialPaths"];
        //メッシュロード時の作られた初期ロットをクリア
        for (auto* mat : mMaterials)
        {
            if (mat) delete mat;
        }
        mMaterials.clear();
        mMaterials.resize(matPaths.size(), nullptr);

        for (size_t i = 0; i < matPaths.size(); ++i)
        {
            string path = matPaths[i].get<string>();
            if (!path.empty())
            {
                mMaterials[i] = MaterialManager::GetMaterial(path);
            }
        }
    }

    // 4. その他のプロパティも読み込む
    if (j.contains("Visible"))
    {
        mVisible = j.at("Visible").get<bool>();
    }
    if (j.contains("IsSkeletal"))
    {
        mIsSkeletal = j.at("IsSkeletal").get<bool>();
    }
    if (j.contains("Alpha"))
    {
        mAlpha = j.at("Alpha").get<float>();
        SetMaterialAlpha(mAlpha);
    }
    if (j.contains("ShadowFrag"))
    {
        mShadowFrag = j.at("ShadowFrag").get<bool>();
    }
}

void MeshRenderer::LoadFilePathAndID(const char* path, const char* localID)
{
    Mesh* mesh = EngineWindow::GetRenderer()->GetSubMesh(path,localID);
    if (mesh)
    {
        SetMesh({mesh});
        mAlpha    = mesh->GetMaterialInfo()[0].Color.w;
        mFilePath = path;
    }
}

void MeshRenderer::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
    ImGui::PushID(this);

    // MeshRendererのプロパティ
    ImGui::Text("Properties");

    ImGui::NewLine();

    // 1.ファイルパスの取得
    filesystem::path currentPath = mFilePath;
    static char pathBuffer[256];
    strncpy_s(pathBuffer, currentPath.filename().stem().string().c_str(),sizeof(pathBuffer));
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
            LoadFilePathAndID(data->sFilePath,data->sLocalID);
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
        //メッシュが持つサブメッシュの数(=マテリアルスロット数)を取得
        int materialCount = mMeshs[0]->GetVertexArrays().size();

        //配列のサイズをスロット数に合わせる
        if (mMaterials.size() != materialCount)
        {
            mMaterials.resize(materialCount, nullptr);
        }

        for (int i = 0; i < materialCount; i++)
        {
            ImGui::PushID(i);
            string matLabel = "Element" + std::to_string(i);

            //割り当てられているか確認してパスを表示
            string displayPath = mMaterials[i] ? mMaterials[i]->GetFilePath() : "None (Mesh Default)";
            filesystem::path p(displayPath);
            displayPath = p.filename().string();//ファイル名だけ表示

            char matBuffer[256];
            strncpy_s(matBuffer, displayPath.c_str(), sizeof(matBuffer));
            matBuffer[sizeof(matBuffer) - 1] = '\0';

            ImGui::InputText(matLabel.c_str(), matBuffer, sizeof(matBuffer),
                             ImGuiInputTextFlags_ReadOnly);

            //ドラッグ&ドロップで.matを割り当てる
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload =
                    ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                {
                    const char* payloadPath = (const char*)payload->Data;
                    filesystem::path droppedPath(payloadPath);

                    if (droppedPath.extension() == ".mat")
                    {
                        mMaterials[i] = MaterialManager::GetMaterial(droppedPath.string());
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
    if (!mMeshs.empty())
    {
        ImGui::Text("Alpha Setting");
        ImGui::SliderFloat("Alpha", &mAlpha, 0.0f, 1.0f, "%.2f");

        if (mAlpha != mMeshs[0]->GetMaterialInfo()[0].Color.w)
        {
            SetMaterialAlpha(mAlpha);
        }
    }

    ImGui::NewLine();

    string shadowText = "Shadow";
    if (mShadowFrag)
    {
        shadowText += "/On";
    }
    else
    {
        shadowText += "/Off";
    }
    ImGui::Checkbox(shadowText.c_str(), &mShadowFrag);

    ImGui::Separator();

    ImGui::PopID();
}

Component* MeshRenderer::Clone(Entity* newOwner) const
{
    // 1. 新しいオーナーを指定して、自分と同じ型のインスタンスを new する
    MeshRenderer* clone = new MeshRenderer(newOwner);

    // 2. 自身のパラメータ（値やリソースへのポインタ）をコピーする
    clone->mVisible    = this->mVisible;
    clone->mFilePath   = this->mFilePath;
    clone->mAlpha      = this->mAlpha;
    clone->mShadowFrag = this->mShadowFrag;
    clone->mMeshs      = this->mMeshs;
    clone->mIsSkeletal = this->mIsSkeletal;

    clone->mMaterials.resize(this->mMaterials.size(), nullptr);
    for (int i = 0; i < this->mMaterials.size(); ++i)
    {
        clone->mMaterials[i] = this->mMaterials[i];
    }

    return clone;
}
