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
    if (ImGui::BeginTable("MaterialSettingsTable", 2))
    //if (ImGui::BeginTable("Material Properties", 2))
    {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed,100.0f);
        ImGui::TableSetupColumn("Control", ImGuiTableColumnFlags_WidthStretch);
        //カラー編集
        ImGuiHelper::TableColorEdit4("BaseColor", &data.sDiffuseColor.x);
        // Shininessの編集
        ImGuiHelper::TableDragFloatHelper("Shininess", &data.sShininess, 0.1f, 0.0f, 128.0f);
        // Metallicの編集
        ImGuiHelper::TableDragFloatHelper("Metallic", &data.sMetallic, 0.01f, 0.0f, 1.0f);
        // Roughnessの編集
        ImGuiHelper::TableDragFloatHelper("Roughness", &data.sRoughness, 0.01f, 0.0f, 1.0f);
        // Emissiveの編集
        ImGuiHelper::TableColorEdit3("Emissive", &data.sEmissive.x);

        // テクスチャパス編集
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Albedo Texture");

        ImGui::TableNextColumn();
        char texBuffer[256];
        strncpy_s(texBuffer, data.sAlbedoTexturePath.c_str(),
                  sizeof(texBuffer));
        texBuffer[sizeof(texBuffer) - 1] = '\0';
        ImGui::InputText("##AlbedoTexture", texBuffer, sizeof(texBuffer),
                         ImGuiInputTextFlags_ReadOnly);
        // 3.ファイルロードボタン(ProjectPanelからのDrag&Dropを想定)
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload =
                    ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                // ペイロードがファイルパスであると仮定
                const char* texPath = (const char*)payload->Data;
                // ファイルパスを使いロード処理を呼び出す
                mCurrentMaterial->SetTexture(
                    EngineWindow::GetRenderer()->GetTexture(texPath));
                data.sAlbedoTexturePath = texPath;
            }
            ImGui::EndDragDropTarget();
        }


        ImGui::EndTable();
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
