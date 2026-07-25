#include "MaterialSettingsItem.h"
#include "MaterialManager.h"
#include "DebugManager.h"
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
    ImGui::ColorEdit4("Diffuse Color", &data.sDiffuseColor.x);
    ImGui::ColorEdit3("Ambient Color", &data.sAmbientColor.x);
    ImGui::ColorEdit3("Specular Color", &data.sSpecularColor.x);
    //数値編集
    ImGui::DragFloat("Shininess", &data.sShininess, 0.1f, 0.0f, 128.0f);

    //テクスチャパス編集
    char texBuffer[256];
    strncpy_s(texBuffer, data.sAlbedoTexturePath.c_str(), sizeof(texBuffer));
    texBuffer[sizeof(texBuffer) - 1] = '\0';
    if (ImGui::InputText("Albedo Texture", texBuffer, sizeof(texBuffer)))
    {
        data.sAlbedoTexturePath = texBuffer;
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
