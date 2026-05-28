#include "SphereCollider.h"
#include "Actor.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

SphereCollider::SphereCollider(Entity* owner, int updateOrder)
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

	Vector3 scale = mActor->GetTransform()->GetLocalScale();
	float maxScale = std::max({ scale.x, scale.y, scale.z });
	
	mWorldSphere.mRadius = mObjectSphere.mRadius * maxScale;

	mWorldSphere.mCenter = mObjectSphere.mCenter * maxScale + mActor->GetTransform()->GetPosition();
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

Component* SphereCollider::Clone(Entity* newOwner) const
{
	SphereCollider* clone = new SphereCollider(newOwner);

	clone->mWorldOBB = this->mWorldOBB;
	clone->mWorldAABB = this->mWorldAABB;
	clone->mIsCollider = this->mIsCollider;
	clone->mContactOffset = this->mContactOffset;

	clone->mObjectSphere = this->mObjectSphere;
	clone->mWorldSphere = this->mWorldSphere;
	clone->mShouldRotate = this->mShouldRotate;
	return clone;
}
