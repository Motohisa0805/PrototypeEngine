#include "SphereCollider.h"
#include "Actor.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

SphereCollider::SphereCollider(ActorObject* owner, int updateOrder)
	:Collider(owner, updateOrder)
	, mObjectSphere(Vector3::Zero, 0.5f)
	, mWorldSphere(Vector3::Zero, 0.5f)
	, mShouldRotate(true)
{
	mName = "SphereCollider";

	mHeaderColor = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
	mHeaderHoveredColor = Vector4(0.3f, 0.6f, 0.3f, 1.0f);
	mHeaderActiveColor = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::OnUpdateWorldTransform()
{
	mWorldSphere = mObjectSphere;

	Vector3 scale = mOwner->GetTransform()->GetLocalScale();
	float maxScale = std::max({ scale.x, scale.y, scale.z });
	
	mWorldSphere.mRadius = mObjectSphere.mRadius * maxScale;

	mWorldSphere.mCenter = mObjectSphere.mCenter * maxScale + mOwner->GetTransform()->GetPosition();
}

OBB SphereCollider::GetWorldOBB() const
{
	return mWorldOBB;
}

AABB SphereCollider::GetWorldAABBFromOBB() const
{
	Vector3 rVec(mWorldSphere.mRadius, mWorldSphere.mRadius, mWorldSphere.mRadius);
	return AABB(
		mWorldSphere.mCenter - rVec,
		mWorldSphere.mCenter + rVec
	);
}

void SphereCollider::Serialize(json& j) const
{
	Collider::Serialize(j);
}

void SphereCollider::Deserialize(const json& j)
{
	Collider::Deserialize(j);
}

void SphereCollider::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
	ImGui::PushID(this);

	ImGui::Text("Properties");
	Collider::DrawCustomGUI(properties);

	ImGui::Separator();

	ImGui::PopID();
}
