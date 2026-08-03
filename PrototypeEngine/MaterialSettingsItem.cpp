#include "MaterialSettingsItem.h"
#include "MaterialManager.h"
#include "DebugManager.h"
#include "Texture.h"
#include "EngineWindow.h"
#include "Renderer.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

filesystem::path MaterialSettingsItem::mCurrentMatPath = "";

Material* MaterialSettingsItem::mCurrentMaterial;

void MaterialSettingsItem::DrawMatSettings(const filesystem::path& matPath) 
{
    if (mCurrentMatPath != matPath)
    {
        mCurrentMatPath = matPath;
        mCurrentMaterial = MaterialManager::GetMaterial(matPath.string());
    }

    if (!mCurrentMaterial)return;

    MaterialData& data = mCurrentMaterial->GetData();

    ImGui::Text("Material Properties");
    ImGui::Separator();
    //カラー編集
    ImGui::ColorEdit4("Base Color", &data.sDiffuseColor.x);

    ImGui::DragFloat("Shininess", &data.sShininess, 0.1f, 0.0f, 128.0f);
    //数値編集
    ImGui::DragFloat("Metallic", &data.sMetallic, 0.01f, 0.0f, 1.0f);

    ImGui::DragFloat("Roughness", &data.sRoughness, 0.01f, 0.0f, 1.0f);

    ImGui::ColorEdit3("Emissive", &data.sEmissive.x);
    //テクスチャパス編集
    char texBuffer[256];
    strncpy_s(texBuffer, data.sAlbedoTexturePath.c_str(), sizeof(texBuffer));
    texBuffer[sizeof(texBuffer) - 1] = '\0';
    ImGui::InputText("Albedo Texture", texBuffer, sizeof(texBuffer),ImGuiInputTextFlags_ReadOnly);
    // 3.ファイルロードボタン(ProjectPanelからのDrag&Dropを想定)
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
        {
            // ペイロードがファイルパスであると仮定
            const char* texPath = (const char*)payload->Data;
            // ファイルパスを使いロード処理を呼び出す
            mCurrentMaterial->SetTexture(EngineWindow::GetRenderer()->GetTexture(texPath));
            data.sAlbedoTexturePath = texPath;
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::NewLine();

    //保存ボタン
    if (ImGui::Button("Save Material", ImVec2(-1.0f, 30.0f)))
    {
        if (mCurrentMaterial->SaveToFile(matPath.string()))
        {
            Debug::Log("Material Asset Saved: %s", matPath.string().c_str());
        }
    }
}
