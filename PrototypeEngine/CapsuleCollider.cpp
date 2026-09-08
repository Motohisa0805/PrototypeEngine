#include "CapsuleCollider.h"
#include "Actor.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"

CapsuleCollider::CapsuleCollider(Entity* owner, int updateOrder)
    : Collider(owner, updateOrder)
    , mObjectCapsule(Vector3::Zero, Vector3::Zero, 0.5f)
    , mWorldCapsule(Vector3::Zero, Vector3::Zero, 0.5f)
    , mShouldRotate(true)
{
    mName = "CapsuleCollider";

    mHeaderColor        = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
    mHeaderHoveredColor = Vector4(0.3f, 0.6f, 0.3f, 1.0f);
    mHeaderActiveColor  = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
}

CapsuleCollider::~CapsuleCollider() {}

void CapsuleCollider::OnUpdateWorldTransform()
{
    mWorldCapsule = mObjectCapsule;

    // スケール・回転・位置を取得
    Vector3    scale = mActor->GetTransform()->GetScale();
    Quaternion rot   = mActor->GetTransform()->GetRotation();
    Vector3    pos   = mActor->GetTransform()->GetPosition();

    // 回転＋スケール → 始点・終点をワールド変換
    Vector3 localStart = mWorldCapsule.sSegment.sStart;
    Vector3 localEnd   = mWorldCapsule.sSegment.sEnd;

    Vector3 rotatedStart = rot.Rotate(localStart);
    Vector3 rotatedEnd   = rot.Rotate(localEnd);

    mWorldCapsule.sSegment.sStart = rotatedStart + pos;
    mWorldCapsule.sSegment.sEnd   = rotatedEnd + pos;

    // 半径スケール（等方スケール想定 or 最大軸スケール使用）
    float uniformScale    = std::max({scale.x, scale.y, scale.z});
    mWorldCapsule.sRadius = mWorldCapsule.sRadius * uniformScale;
}

OBB CapsuleCollider::GetWorldOBB() const { return mWorldOBB; }

AABB CapsuleCollider::GetWorldAABBFromOBB() const
{
    const Vector3& p0 = mWorldCapsule.sSegment.sStart;
    const Vector3& p1 = mWorldCapsule.sSegment.sEnd;
    float          r  = mWorldCapsule.sRadius;

    Vector3 min = Vector3::Min(p0, p1);
    Vector3 max = Vector3::Max(p0, p1);

    Vector3 rVec(r, r, r);
    return AABB(min - rVec, max + rVec);
}

void CapsuleCollider::Serialize(json& j) const
{
    Collider::Serialize(j);
    j["Radius"] = mObjectCapsule.sRadius;

    j["mObjectCapsule.mSegment.mStart"] = {mObjectCapsule.sSegment.sStart.x,
                                           mObjectCapsule.sSegment.sStart.y,
                                           mObjectCapsule.sSegment.sStart.z};
    j["mObjectCapsule.mSegment.mEnd"]   = {mObjectCapsule.sSegment.sEnd.x,
                                           mObjectCapsule.sSegment.sEnd.y,
                                           mObjectCapsule.sSegment.sEnd.z};
}

void CapsuleCollider::Deserialize(const json& j)
{
    Collider::Deserialize(j);

    if (j.contains("Radius"))
    {
        mObjectCapsule.sRadius = j.at("Radius").get<float>();
    }

    if (j.contains("mObjectCapsule.mSegment.mStart"))
    {
        mObjectCapsule.sSegment.sStart =
            Vector3(j["mObjectCapsule.mSegment.mStart"][0],
                    j["mObjectCapsule.mSegment.mStart"][1],
                    j["mObjectCapsule.mSegment.mStart"][2]);
    }

    if (j.contains("mObjectCapsule.mSegment.mEnd"))
    {
        mObjectCapsule.sSegment.sEnd =
            Vector3(j["mObjectCapsule.mSegment.mEnd"][0],
                    j["mObjectCapsule.mSegment.mEnd"][1],
                    j["mObjectCapsule.mSegment.mEnd"][2]);
    }
}

void CapsuleCollider::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
    ImGui::PushID(this);

    ImGui::Text("Properties");
    Collider::DrawCustomGUI(properties);

    ImGui::SetNextItemWidth(50);
    ImGui::DragFloat("Radius", &mObjectCapsule.sRadius);

    ImGui::Text("Start");
    ImGui::SameLine();
    ImGui::DragFloat3("##start", &mObjectCapsule.sSegment.sStart.x);

    ImGui::Text("End");
    ImGui::SameLine();
    ImGui::DragFloat3("##end", &mObjectCapsule.sSegment.sEnd.x);

    ImGui::Separator();

    ImGui::PopID();
}

Component* CapsuleCollider::Clone(Entity* newOwner) const
{
    CapsuleCollider* clone = new CapsuleCollider(newOwner);

    clone->mWorldOBB   = this->mWorldOBB;
    clone->mWorldAABB  = this->mWorldAABB;
    clone->mIsCollider = this->mIsCollider;

    clone->mObjectCapsule = this->mObjectCapsule;
    clone->mWorldCapsule  = this->mWorldCapsule;
    clone->mShouldRotate  = this->mShouldRotate;

    return clone;
}
