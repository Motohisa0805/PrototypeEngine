#include "SphereCollider.h"
#include "Actor.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"

SphereCollider::SphereCollider(Entity* owner, int updateOrder)
    : Collider(owner, updateOrder)
    , mObjectSphere(Vector3::Zero, 0.5f)
    , mWorldSphere(Vector3::Zero, 0.5f)
    , mShouldRotate(true)
{
    mName = "SphereCollider";

    mHeaderColor        = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
    mHeaderHoveredColor = Vector4(0.3f, 0.6f, 0.3f, 1.0f);
    mHeaderActiveColor  = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
}

SphereCollider::~SphereCollider() {}

void SphereCollider::OnUpdateWorldTransform()
{
    mWorldSphere = mObjectSphere;

    Vector3 scale    = mActor->GetTransform()->GetLocalScale();
    float   maxScale = std::max({scale.x, scale.y, scale.z});

    mWorldSphere.sRadius = mObjectSphere.sRadius * maxScale;

    mWorldSphere.sCenter = mObjectSphere.sCenter * maxScale +
                           mActor->GetTransform()->GetPosition();
}

OBB SphereCollider::GetWorldOBB() const { return mWorldOBB; }

AABB SphereCollider::GetWorldAABBFromOBB() const
{
    Vector3 rVec(mWorldSphere.sRadius, mWorldSphere.sRadius,
                 mWorldSphere.sRadius);
    return AABB(mWorldSphere.sCenter - rVec, mWorldSphere.sCenter + rVec);
}

void SphereCollider::Serialize(json& j) const
{
    Collider::Serialize(j);
    j["Radius"] = mObjectSphere.sRadius;

    j["Center"] = {mObjectSphere.sCenter.x, mObjectSphere.sCenter.y,
                   mObjectSphere.sCenter.z};
}

void SphereCollider::Deserialize(const json& j)
{
    Collider::Deserialize(j);
    if (j.contains("Radius"))
    {
        mObjectSphere.sRadius = j.at("Radius").get<float>();
    }

    if (j.contains("Center"))
    {
        mObjectSphere.sCenter =
            Vector3(j["Center"][0], j["Center"][1], j["Center"][2]);
    }
}

void SphereCollider::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
    ImGui::PushID(this);

    ImGui::Text("Properties");
    Collider::DrawCustomGUI(properties);
    ImGui::SetNextItemWidth(50);
    ImGui::DragFloat("Radius", &mObjectSphere.sRadius);

    ImGui::Text("Center");
    ImGui::DragFloat3("##center", &mObjectSphere.sCenter.x);

    ImGui::Separator();

    ImGui::PopID();
}

Component* SphereCollider::Clone(Entity* newOwner) const
{
    SphereCollider* clone = new SphereCollider(newOwner);

    clone->mWorldOBB   = this->mWorldOBB;
    clone->mWorldAABB  = this->mWorldAABB;
    clone->mIsCollider = this->mIsCollider;

    clone->mObjectSphere = this->mObjectSphere;
    clone->mWorldSphere  = this->mWorldSphere;
    clone->mShouldRotate = this->mShouldRotate;
    return clone;
}
