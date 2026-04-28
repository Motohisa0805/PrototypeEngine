#include "DirectionalLightComponent.h"
#include "EngineWindow.h"
#include "Renderer.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

DirectionalLightComponent::DirectionalLightComponent(ActorObject* owner)
	: Component(owner)
	, mDirectionalLight()
{
    mName = "DirectionalLightComponent";
    mDirectionalLight.gDirection = Vector3(0.0f, -0.707f, -0.707f);
    mDirectionalLight.gDiffuseColor = Vector3(0.78f, 0.88f, 1.0f);
    mDirectionalLight.gSpecColor = Vector3(0.8f, 0.8f, 0.8f);

    mHeaderColor = Vector4(0.4f, 0.8f, 0.8f, 1.0f);
    mHeaderHoveredColor = Vector4(0.3f, 0.6f, 0.6f, 1.0f);
    mHeaderActiveColor = Vector4(0.4f, 0.8f, 0.8f, 1.0f);
}

DirectionalLightComponent::~DirectionalLightComponent()
{
}

void DirectionalLightComponent::OnUpdateWorldTransform()
{
    // ライトベクトルの基準軸（太陽が右から左に動く＝X軸方向）
    Vector3 baseDir = Vector3::UnitX;

    // 回転を適用してライトの最終的な方向を得る（ワールド空間）
    Vector3 lightDir = Vector3::Transform(baseDir, mOwner->GetTransform()->GetRotation());
    //lightDir.Normalize();

    // ライト方向を保存
    mDirectionalLight.gDirection = lightDir;


    // ライトのY成分（太陽の高さ）を使って環境光を調整
    float y = Math::Clamp(lightDir.y, 0.0f, 1.0f);

    // Ambient（環境光）
    Vector3 dayAmbient = Vector3(0.7f, 0.7f, 0.6f);
    Vector3 nightAmbient = Vector3(0.05f, 0.05f, 0.1f);
    mDirectionalLight.gAmbientColor = Vector3::Lerp(dayAmbient, nightAmbient, y);

    // Diffuse（拡散光）
    Vector3 dayDiffuse = Vector3(1.0f, 0.95f, 0.8f);
    Vector3 nightDiffuse = Vector3(0.05f, 0.05f, 0.1f);
    mDirectionalLight.gDiffuseColor = Vector3::Lerp(dayDiffuse, nightDiffuse, y);

    // Specular（鏡面反射）
    Vector3 daySpecular = Vector3(1.0f, 1.0f, 1.0f);
    Vector3 nightSpecular = Vector3(0.0f, 0.0f, 0.0f);
    mDirectionalLight.gSpecColor = Vector3::Lerp(daySpecular, nightSpecular, y);



    mDirectionalLight.gPosition = mOwner->GetTransform()->GetPosition();
    EngineWindow::GetRenderer()->SetDirectionalLight(mDirectionalLight);
}

void DirectionalLightComponent::Serialize(json& j) const
{
	Component::Serialize(j);

	j["Direction"] = { mDirectionalLight.gDirection.x, mDirectionalLight.gDirection.y, mDirectionalLight.gDirection.z };
	j["DiffuseColor"] = { mDirectionalLight.gDiffuseColor.x, mDirectionalLight.gDiffuseColor.y, mDirectionalLight.gDiffuseColor.z };
	j["AmbientColor"] = { mDirectionalLight.gAmbientColor.x, mDirectionalLight.gAmbientColor.y, mDirectionalLight.gAmbientColor.z };
	j["SpecularColor"] = { mDirectionalLight.gSpecColor.x, mDirectionalLight.gSpecColor.y, mDirectionalLight.gSpecColor.z };
}

void DirectionalLightComponent::Deserialize(const json& j)
{
	Component::Deserialize(j);
    if (j.contains("Direction"))
    {
        auto dirArray = j["Direction"];
        mDirectionalLight.gDirection.Set(dirArray[0], dirArray[1], dirArray[2]);
	}
    if (j.contains("DiffuseColor"))
    {
        auto diffArray = j["DiffuseColor"];
        mDirectionalLight.gDiffuseColor.Set(diffArray[0], diffArray[1], diffArray[2]);
    }
    if (j.contains("AmbientColor"))
    {
        auto ambArray = j["AmbientColor"];
        mDirectionalLight.gAmbientColor.Set(ambArray[0], ambArray[1], ambArray[2]);
    }
    if (j.contains("SpecularColor"))
    {
        auto specArray = j["SpecularColor"];
        mDirectionalLight.gSpecColor.Set(specArray[0], specArray[1], specArray[2]);
	}
}

void DirectionalLightComponent::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
	ImGui::PushID(this);

    ImGui::Text("DirectionalLight Component");

    ImGui::NewLine();
    
    ImGui::Text("Direction");
	ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("##direction", &mDirectionalLight.gDirection.x, 0.1f, -1.0f, 1.0f);
    
    ImGui::NewLine();
    
    ImGui::Text("Diffuse Color");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("##diffuse Color", &mDirectionalLight.gDiffuseColor.x, 0.01f, 0.0f, 1.0f);
	
    ImGui::NewLine();
    
    ImGui::Text("Ambient Color");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("##ambient Color", &mDirectionalLight.gAmbientColor.x, 0.01f, 0.0f, 1.0f);
	
    ImGui::NewLine();
    
    ImGui::Text("Specular Color");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
	ImGui::DragFloat3("##specular Color", &mDirectionalLight.gSpecColor.x, 0.01f, 0.0f, 1.0f);
	
    ImGui::NewLine();
	
    ImGui::Separator();

    ImGui::PopID();
}
