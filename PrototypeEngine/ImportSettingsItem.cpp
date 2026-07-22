#include "ImportSettingsItem.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "AssetImporter.h"

ImportSettingsItem::ImportTab ImportSettingsItem::mCurrentTab = ImportSettingsItem::ImportTab::Model;

filesystem::path ImportSettingsItem::mCurrentFBXPath = "";

AllImportSettings ImportSettingsItem::mCurrentAllImportSettings;

void ImportSettingsItem::DrawModelSettings() {
    ImGui::Text("Scene"); 
    ImGui::DragFloat("ScaleFactor",
                     &mCurrentAllImportSettings.sModel.sScaleFactory);
    ImGui::Checkbox("ConvertUnits", &mCurrentAllImportSettings.sModel.sConvertUnits);
    
    ImGui::Checkbox("BakeAxisConversion", &mCurrentAllImportSettings.sModel.sBakeAxisConversion);
    
    ImGui::Checkbox("ImportBlendShapes", &mCurrentAllImportSettings.sModel.sImportBlendShapes);
    
    ImGui::Checkbox("ImportDeformPercent", &mCurrentAllImportSettings.sModel.sImportDeformPercent);
    
    ImGui::Checkbox("ImportVisibility", &mCurrentAllImportSettings.sModel.sImportVisibility);
    
    ImGui::Checkbox("ImportCameras", &mCurrentAllImportSettings.sModel.sImportCameras);
    
    ImGui::Checkbox("ImportLights", &mCurrentAllImportSettings.sModel.sImportLights);
    
    ImGui::Checkbox("PreserveHierarchy", &mCurrentAllImportSettings.sModel.sPreserveHierarchy);
    
    ImGui::Checkbox("SortHierarchyByName", &mCurrentAllImportSettings.sModel.sSortHierarchyByName);
}

void ImportSettingsItem::DrawRigSettings() {}

void ImportSettingsItem::DrawAnimationSettings() {}

void ImportSettingsItem::DrawMaterialsSettings() 
{
    ImGui::Text("Materials");
    ImGui::SameLine();
    if (ImGui::Button("Extract Materials...", ImVec2(-1.0f, 30.0f)))
    {
        //モデルのマテリアルを取り出す処理
    }
}

void ImportSettingsItem::DrawFBXImportSettings(const filesystem::path& fbxPath)
{
    //fbxファイルからmetaファイルを読み込み
    if (mCurrentFBXPath != fbxPath)
    {
        mCurrentAllImportSettings = AssetImporter::OutputFBXMetaFile(fbxPath);
        mCurrentFBXPath = fbxPath;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    //ウィンドウの幅を取得し、4つのボタンで等分
    float windowWidth = ImGui::GetContentRegionAvail().x;
    float buttonWidth = windowWidth / 4.0f;
    ImVec2 buttonSize(buttonWidth, 30.0f);

    const char* tabNames[] = {"Model", "Rig", "Animation", "Materials"};
    ImportTab   tabValues[] = {ImportTab::Model,ImportTab::Rig,ImportTab::Animation,ImportTab::Materials};

    for (int i = 0; i < 4; ++i)
    {
        bool isSelected = (mCurrentTab == tabValues[i]);
        if (isSelected)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        }

        //ボタンの描画
        if (ImGui::Button(tabNames[i], buttonSize))
        {
            mCurrentTab = tabValues[i];
        }

        //変更した色を元に戻す
        if (isSelected)
        {
            ImGui::PopStyleColor(2);
        }

        if (i < 3)
        {
            ImGui::SameLine(0.0f, 0.0f);
        }
    }

    ImGui::PopStyleVar();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    switch (mCurrentTab)
    {
    case ImportSettingsItem::ImportTab::Model:
        ImGui::Text("Model Import Options");
        DrawModelSettings();
        break;
    case ImportSettingsItem::ImportTab::Rig:
        ImGui::Text("Rig Import Options");
        DrawRigSettings();
        break;
    case ImportSettingsItem::ImportTab::Animation:
        ImGui::Text("Animation Import Options");
        DrawAnimationSettings();
        break;
    case ImportSettingsItem::ImportTab::Materials:
        ImGui::Text("Material Import Options");
        DrawMaterialsSettings();
        break;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Apply", ImVec2(-1.0f, 30.0f)))
    {
    }
}