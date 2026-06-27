#include "DirectionalLightComponent.h"
#include "EngineWindow.h"
#include "Renderer.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

DirectionalLightComponent::DirectionalLightComponent(Entity* owner)
	: Component(owner)
	, mDirectionalLight()
{
    mName = "DirectionalLight";

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
    Vector3 baseDir = Vector3::UnitZ;

    // 回転を適用してライトの最終的な方向を得る（ワールド空間）
    Vector3 lightDir = Vector3::Transform(baseDir, mActor->GetTransform()->GetRotation());
    lightDir.Normalize();

    // ライト方向を保存
    mDirectionalLight.sDirection = lightDir;


    // ライトのY成分（太陽の高さ）を使って環境光を調整
    float dayFactor = Math::Clamp(-lightDir.y, 0.0f, 1.0f);

    // Diffuse（拡散光）
    Vector3 dayDiffuse = Vector3(1.0f, 0.95f, 0.8f);
    Vector3 nightDiffuse = Vector3(0.05f, 0.05f, 0.1f);
    mDirectionalLight.sDiffuseColor = Vector3::Lerp(nightDiffuse, dayDiffuse, dayFactor);

    // Specular（鏡面反射）
    Vector3 daySpecular = Vector3(1.0f, 1.0f, 1.0f);
    Vector3 nightSpecular = Vector3(0.0f, 0.0f, 0.0f);
    mDirectionalLight.sSpecColor = Vector3::Lerp(nightSpecular, daySpecular, dayFactor);



    mDirectionalLight.sPosition = mActor->GetTransform()->GetPosition();
    EngineWindow::GetRenderer()->SetDirectionalLight(mDirectionalLight);
}

void DirectionalLightComponent::Serialize(json& j) const
{
	Component::Serialize(j);

	j["Direction"] = { mDirectionalLight.sDirection.x, mDirectionalLight.sDirection.y, mDirectionalLight.sDirection.z };
	j["DiffuseColor"] = { mDirectionalLight.sDiffuseColor.x, mDirectionalLight.sDiffuseColor.y, mDirectionalLight.sDiffuseColor.z };
	j["AmbientColor"] = { mDirectionalLight.sAmbientColor.x, mDirectionalLight.sAmbientColor.y, mDirectionalLight.sAmbientColor.z };
    j["AmbientIntensity"] = mDirectionalLight.sAmbientIntensity;
	j["SpecularColor"] = { mDirectionalLight.sSpecColor.x, mDirectionalLight.sSpecColor.y, mDirectionalLight.sSpecColor.z };
}

void DirectionalLightComponent::Deserialize(const json& j)
{
	Component::Deserialize(j);
    if (j.contains("Direction"))
    {
        auto dirArray = j["Direction"];
        mDirectionalLight.sDirection.Set(dirArray[0], dirArray[1], dirArray[2]);
	}
    if (j.contains("DiffuseColor"))
    {
        auto diffArray = j["DiffuseColor"];
        mDirectionalLight.sDiffuseColor.Set(diffArray[0], diffArray[1], diffArray[2]);
    }
    if (j.contains("AmbientColor"))
    {
        auto ambArray = j["AmbientColor"];
        mDirectionalLight.sAmbientColor.Set(ambArray[0], ambArray[1], ambArray[2]);
    }
    if (j.contains("AmbientIntensity")) {
        auto ambInt = j.at("AmbientIntensity").get<float>();
        mDirectionalLight.sAmbientIntensity = ambInt;
    }
    if (j.contains("SpecularColor"))
    {
        auto specArray = j["SpecularColor"];
        mDirectionalLight.sSpecColor.Set(specArray[0], specArray[1], specArray[2]);
	}
}

void DirectionalLightComponent::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
	ImGui::PushID(this);

    ImGui::Separator();

    ImGui::Text("Ambient Color");
    ImGui::SetNextItemWidth(200);
    if (ImGui::ColorEdit3("##ambientColor", &mDirectionalLight.sAmbientColor.x)) {
        mActor->GetTransform()->SetDirty();
    }

    ImGui::Text("Ambient Intensity");
    if (ImGui::SliderFloat("##ambientIntensity", &mDirectionalLight.sAmbientIntensity, 0.0f, 5.0f)) {
        mActor->GetTransform()->SetDirty();
    }

    ImGui::Separator();

    ImGui::PopID();
}

Component* DirectionalLightComponent::Clone(Entity* newOwner) const
{
    DirectionalLightComponent* clone = new DirectionalLightComponent(newOwner);

    clone->mDirectionalLight = this->mDirectionalLight;

    return clone;
}
