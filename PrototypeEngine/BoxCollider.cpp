#include "BoxCollider.h"
#include "Actor.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"

BoxCollider::BoxCollider(Entity* owner, int updateOrder)
    : Collider(owner, updateOrder)
    , mObjectAABB(Vector3::Zero, Vector3::Zero)
    , mObjectOBB(Vector3::Zero, Quaternion::Identity, Vector3::Zero)
    , mShouldRotate(true)
{
    mName = "BoxCollider";
    // 単位ボックスを基準とした OBB
    mObjectOBB.sCenter   = mActor->GetTransform()->GetPosition();
    mObjectOBB.sRotation = mActor->GetTransform()->GetRotation();
    mObjectOBB.sExtents  = Vector3(0.5f, 0.5f, 0.5f); // 1x1x1ボックスの半分

    mHeaderColor        = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
    mHeaderHoveredColor = Vector4(0.3f, 0.6f, 0.3f, 1.0f);
    mHeaderActiveColor  = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
}

BoxCollider::~BoxCollider() {}

void BoxCollider::OnUpdateWorldTransform()
{
    //===OBBの更新===
    // スケール、回転、位置を取得
    Vector3    scale    = mActor->GetTransform()->GetScale();
    Quaternion rotation = mActor->GetTransform()->GetRotation();
    Vector3    position = mActor->GetTransform()->GetPosition();
    Vector3    offset   = mObjectOBB.sOffset;

    // オフセットにスケールを適用
    Vector3 scaledOffset =
        Vector3(mObjectOBB.sOffset.x * scale.x, mObjectOBB.sOffset.y * scale.y,
                mObjectOBB.sOffset.z * scale.z);

    // ワールド OBB を構築
    // スケールされたオフセットを回転させてから、位置に加算する
    mWorldOBB.sCenter = position + Vector3::Transform(scaledOffset, rotation);

    mWorldOBB.sRotation = rotation;

    mWorldOBB.sExtents = Vector3(mObjectOBB.sExtents.x * std::abs(scale.x),
                                 mObjectOBB.sExtents.y * std::abs(scale.y),
                                 mObjectOBB.sExtents.z * std::abs(scale.z));
    //===AABBの更新===
    mWorldAABB = GetWorldAABBFromOBB();
}

AABB BoxCollider::GetWorldAABBFromOBB() const
{
    const OBB& obb = mWorldOBB;

    Vector3 x =
        Vector3::Transform(Vector3::UnitX, obb.sRotation) * obb.sExtents.x;
    Vector3 y =
        Vector3::Transform(Vector3::UnitY, obb.sRotation) * obb.sExtents.y;
    Vector3 z =
        Vector3::Transform(Vector3::UnitZ, obb.sRotation) * obb.sExtents.z;

    Vector3 r = Vector3::Abs(x) + Vector3::Abs(y) + Vector3::Abs(z);
    return AABB(obb.sCenter - r, obb.sCenter + r);
}

void BoxCollider::Serialize(json& j) const
{
    Collider::Serialize(j);
    j["mObjectOBB.mOffset"] = {mObjectOBB.sOffset.x, mObjectOBB.sOffset.y,
                               mObjectOBB.sOffset.z};
}

void BoxCollider::Deserialize(const json& j)
{
    Collider::Deserialize(j);
    if (j.contains("mObjectOBB.mOffset")) // contains() の方が意図が明確
    {
        const auto& offsetArray = j.at("mObjectOBB.mOffset");

        // 1. 配列であるか、かつ 2. 要素数が3つ以上であるかをチェック
        if (offsetArray.is_array() && offsetArray.size() >= 3)
        {
            try
            {
                // 各要素が数値であるかを確認しつつ代入
                if (offsetArray.at(0).is_number() &&
                    offsetArray.at(1).is_number() &&
                    offsetArray.at(2).is_number())
                {
                    mObjectOBB.sOffset.x = offsetArray.at(0).get<float>();
                    mObjectOBB.sOffset.y = offsetArray.at(1).get<float>();
                    mObjectOBB.sOffset.z = offsetArray.at(2).get<float>();
                }
                else
                {
                    // エラー処理: 要素が数値ではなかった場合
                    std::cerr
                        << "Error: mOffset array elements are not numbers."
                        << std::endl;
                }
            }
            catch (const json::type_error& e)
            {
                // get<float>() で変換に失敗した場合など
                std::cerr << "Error: JSON type error during float conversion: "
                          << e.what() << std::endl;
            }
        }
        else
        {
            // エラー処理: 配列でない、または要素数が不足している場合
            std::cerr
                << "Error: mObjectOBB.mOffset is not a valid array of size 3."
                << std::endl;
        }
    }
}

void BoxCollider::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
    ImGui::PushID(this);

    ImGui::Text("Properties");
    Collider::DrawCustomGUI(properties);

    ImGui::NewLine();

    ImGui::Text("Offset");
    ImGui::SameLine();
    ImGui::DragFloat3("##offset", &mObjectOBB.sOffset.x);
    ImGui::Text("Extents");
    ImGui::SameLine();
    ImGui::DragFloat3("##extents", &mObjectOBB.sExtents.x);

    ImGui::NewLine();

    ImGui::Separator();

    ImGui::PopID();
}

Component* BoxCollider::Clone(Entity* newOwner) const
{
    BoxCollider* clone = new BoxCollider(newOwner);

    clone->mWorldOBB   = this->mWorldOBB;
    clone->mWorldAABB  = this->mWorldAABB;
    clone->mIsCollider = this->mIsCollider;

    clone->mObjectAABB   = this->mObjectAABB;
    clone->mObjectOBB    = this->mObjectOBB;
    clone->mShouldRotate = this->mShouldRotate;
    return clone;
}
