#include "LocalLight.h"
#include "Actor.h"
#include "BaseScene.h"
#include "EngineWindow.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"

LocalLight::LocalLight(Entity* owner)
    : Component(owner)
    , mLightType(LightType::Point)
    , mColor(Vector3())
    , mRange(1.0f)
    , mIntensity(1.0f)
    , mAngles(1.0f)
{
    mName = "LocalLight";

    EngineWindow::GetRenderer()->AddPointLight(this);

    mHeaderColor        = Vector4(0.4f, 0.8f, 0.8f, 1.0f);
    mHeaderHoveredColor = Vector4(0.3f, 0.6f, 0.6f, 1.0f);
    mHeaderActiveColor  = Vector4(0.4f, 0.8f, 0.8f, 1.0f);
}

LocalLight::~LocalLight()
{
    EngineWindow::GetRenderer()->RemovePointLight(this);
}

string LocalLight::GetLightComponentName(LightType type)
{
    switch (type)
    {
    case LocalLight::Point:
        return "PointLight";
    case LocalLight::Spot:
        return "SpotLight";
    case LocalLight::Area:
        return "AreaLight";
    }
    return "NoLight";
}

Vector2 LocalLight::GetAngles()
{
    Vector3 spotDir    = mOwner->GetBaseTransform()->GetForward();
    float   innerAngle = 12.5f; // “àŠp
    float   outerAngle = 17.5f; // ŠOŠp

    float cosInner = std::cos(innerAngle * (Math::Pi / 180.0f));
    float cosOuter = std::cos(outerAngle * (Math::Pi / 180.0f));

    return Vector2(cosInner, cosOuter);
}

void LocalLight::Serialize(json& j) const
{
    Component::Serialize(j);

    j["LightType"] = mLightType;

    j["Color"]     = {mColor.x, mColor.y, mColor.z};
    j["Range"]     = mRange;
    j["Intensity"] = mIntensity;

    j["Angles"] = mAngles;
}

void LocalLight::Deserialize(const json& j)
{
    Component::Deserialize(j);

    if (j.contains("LightType"))
    {
        auto type  = j.at("LightType").get<LightType>();
        mLightType = type;
    }

    if (j.contains("Color"))
    {
        auto colorArray = j["Color"];
        mColor.Set(colorArray[0], colorArray[1], colorArray[2]);
    }
    if (j.contains("Range"))
    {
        auto range = j.at("Range").get<float>();
        mRange     = range;
    }
    if (j.contains("Intensity"))
    {
        auto intensity = j.at("Intensity").get<float>();
        mIntensity     = intensity;
    }

    if (j.contains("Angles"))
    {
        auto angles = j.at("Angles").get<float>();
        mAngles     = angles;
    }
}

void LocalLight::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
    ImGui::PushID(this);

    ImGui::Separator();

    ImGuiTableFlags tableFlags =
    ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp;

    if (ImGui::BeginTable("LocalLightSettingsTable", 2, tableFlags))
    {
        ImGui::TableNextRow();

        if (ImGui::BeginCombo("LightType",
                              GetLightComponentName(mLightType).c_str()))
        {
            for (uint32_t i = 0; i < LightType::Count; ++i)
            {
                LightType tag        = static_cast<LightType>(i);
                bool      isSelected = (mLightType == tag);

                if (ImGui::Selectable(GetLightComponentName(tag).c_str(),
                                      isSelected))
                {
                    mLightType = tag;
                }
            }
            ImGui::EndCombo();
        }

        if (ImGuiHelper::TableColorEdit3("Light Color", &mColor.x))
        {
            mActor->GetTransform()->SetDirty();
        }

        if (ImGuiHelper::TableSliderFloat("Intensity", &mIntensity, 0.0f, 5.0f))
        {
            mActor->GetTransform()->SetDirty();
        }

        if (ImGuiHelper::TableDragFloatHelper("Light Range", &mRange))
        {
            mActor->GetTransform()->SetDirty();
        }

        ImGui::EndTable();
    }


    ImGui::Separator();

    ImGui::PopID();
}

Component* LocalLight::Clone(Entity* newOwner) const
{
    LocalLight* clone = new LocalLight(newOwner);

    clone->mLightType = this->mLightType;

    clone->mColor     = this->mColor;
    clone->mRange     = this->mRange;
    clone->mIntensity = this->mIntensity;
    return clone;
}
