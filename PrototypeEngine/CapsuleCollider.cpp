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
    Vector3 localStart = mWorldCapsule.mSegment.mStart;
    Vector3 localEnd   = mWorldCapsule.mSegment.mEnd;

    Vector3 rotatedStart = rot.Rotate(localStart);
    Vector3 rotatedEnd   = rot.Rotate(localEnd);

    mWorldCapsule.mSegment.mStart = rotatedStart + pos;
    mWorldCapsule.mSegment.mEnd   = rotatedEnd + pos;

    // 半径スケール（等方スケール想定 or 最大軸スケール使用）
    float uniformScale    = std::max({scale.x, scale.y, scale.z});
    mWorldCapsule.mRadius = mWorldCapsule.mRadius * uniformScale;
}

OBB CapsuleCollider::GetWorldOBB() const { return mWorldOBB; }

AABB CapsuleCollider::GetWorldAABBFromOBB() const
{
    const Vector3& p0 = mWorldCapsule.mSegment.mStart;
    const Vector3& p1 = mWorldCapsule.mSegment.mEnd;
    float          r  = mWorldCapsule.mRadius;

    Vector3 min = Vector3::Min(p0, p1);
    Vector3 max = Vector3::Max(p0, p1);

    Vector3 rVec(r, r, r);
    return AABB(min - rVec, max + rVec);
}

void CapsuleCollider::Serialize(json& j) const
{
    Collider::Serialize(j);
    j["Radius"] = mObjectCapsule.mRadius;

    j["mObjectCapsule.mSegment.mStart"] = {mObjectCapsule.mSegment.mStart.x,
                                           mObjectCapsule.mSegment.mStart.y,
                                           mObjectCapsule.mSegment.mStart.z};
    j["mObjectCapsule.mSegment.mEnd"]   = {mObjectCapsule.mSegment.mEnd.x,
                                           mObjectCapsule.mSegment.mEnd.y,
                                           mObjectCapsule.mSegment.mEnd.z};
}

void CapsuleCollider::Deserialize(const json& j)
{
    Collider::Deserialize(j);

    if (j.contains("Radius"))
    {
        mObjectCapsule.mRadius = j.at("Radius").get<float>();
    }

    if (j.contains("mObjectCapsule.mSegment.mStart"))
    {
        mObjectCapsule.mSegment.mStart =
            Vector3(j["mObjectCapsule.mSegment.mStart"][0],
                    j["mObjectCapsule.mSegment.mStart"][1],
                    j["mObjectCapsule.mSegment.mStart"][2]);
    }

    if (j.contains("mObjectCapsule.mSegment.mEnd"))
    {
        mObjectCapsule.mSegment.mEnd =
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
    ImGui::DragFloat("Radius", &mObjectCapsule.mRadius);

    ImGui::Text("Start");
    ImGui::SameLine();
    ImGui::DragFloat3("##start", &mObjectCapsule.mSegment.mStart.x);

    ImGui::Text("End");
    ImGui::SameLine();
    ImGui::DragFloat3("##end", &mObjectCapsule.mSegment.mEnd.x);

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
