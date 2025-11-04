#include "BoxCollider.h"
#include "Actor.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

BoxCollider::BoxCollider(ActorObject* owner, int updateOrder)
	:Collider(owner, updateOrder)
	, mObjectBox(Vector3::Zero, Vector3::Zero)
	, mObjectOBB(Vector3::Zero, Quaternion::Identity, Vector3::Zero)
	, mShouldRotate(true)
{
	mName = "BoxCollider";
	// 単位ボックスを基準とした OBB
	mObjectOBB.mCenter = owner->GetPosition();
	mObjectOBB.mRotation = owner->GetRotation();
	mObjectOBB.mExtents = Vector3(0.5f, 0.5f, 0.5f); // 1x1x1ボックスの半分


	mHeaderColor = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
	mHeaderHoveredColor = Vector4(0.3f, 0.6f, 0.3f, 1.0f);
	mHeaderActiveColor = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
}

BoxCollider::~BoxCollider()
{

}

void BoxCollider::OnUpdateWorldTransform()
{
	//===OBBの更新===
	// スケール、回転、位置を取得
	Vector3 scale = mOwner->GetScale();
	Quaternion rotation = mOwner->GetRotation();
	Vector3 position = mOwner->GetPosition();
	Vector3 offset = mObjectOBB.mOffset;

	// ワールド OBB を構築
	mWorldOBB.mCenter = position + offset;
	mWorldOBB.mRotation = rotation;
	mWorldOBB.mExtents = mObjectOBB.mExtents * scale;

	//===AABBの更新===
	// オブジェクトボックスにリセットする
	mWorldBox = mObjectBox;
	// Scale
	mWorldBox.mMin *= mOwner->GetScale();
	mWorldBox.mMax *= mOwner->GetScale();
	// Rotate (if we want to)
	if (mShouldRotate)
	{
		mWorldBox.Rotate(mOwner->GetRotation());
	}
	// Translate
	mWorldBox.mMin += mOwner->GetPosition();
	mWorldBox.mMax += mOwner->GetPosition();
}

AABB BoxCollider::GetWorldAABBFromOBB() const
{
	const OBB& obb = mWorldOBB;

	Vector3 x = Vector3::Transform(Vector3::UnitX, obb.mRotation) * obb.mExtents.x;
	Vector3 y = Vector3::Transform(Vector3::UnitY, obb.mRotation) * obb.mExtents.y;
	Vector3 z = Vector3::Transform(Vector3::UnitZ, obb.mRotation) * obb.mExtents.z;

	Vector3 r = Vector3::Abs(x) + Vector3::Abs(y) + Vector3::Abs(z);
	return AABB(obb.mCenter - r, obb.mCenter + r);
}

void BoxCollider::Serialize(json& j) const
{
	Collider::Serialize(j);
	j["mObjectOBB.mOffset"] = { mObjectOBB.mOffset.x, mObjectOBB.mOffset.y, mObjectOBB.mOffset.z };
}

void BoxCollider::Deserialize(const json& j)
{
	Collider::Deserialize(j);
	auto offsetArray = j.at("mObjectOBB.mOffset");
	mObjectOBB.mOffset.x = offsetArray.at(0).get<float>();
	mObjectOBB.mOffset.y = offsetArray.at(1).get<float>();
	mObjectOBB.mOffset.z = offsetArray.at(2).get<float>();
}

void BoxCollider::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
	ImGui::Text("BoxCollider Properties");
	Collider::DrawCustomGUI(properties);

	ImGui::DragFloat3("Offset", &mObjectOBB.mOffset.x);
	ImGui::NewLine();

	ImGui::Separator();
}
