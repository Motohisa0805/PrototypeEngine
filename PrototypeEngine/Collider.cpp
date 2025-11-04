#include "Collider.h"
#include "Actor.h"
#include "BaseScene.h"
#include "PhysWorld.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

Collider::Collider(ActorObject* owner, int updateOrder)
	:Component(owner,updateOrder)
	, mWorldBox(Vector3::Zero, Vector3::Zero)
	, mIsCollider(true)
	, mStaticObject(true)
	, mContactOffset(0.001f)
	, mWorldOBB(Vector3::Zero, Quaternion::Identity, Vector3::Zero)
{
	mOwner->GetGame()->GetPhysWorld()->AddCollider(this);
}

Collider::~Collider()
{
	mOwner->GetGame()->GetPhysWorld()->RemoveCollider(this);
}

void Collider::Serialize(json& j) const
{
	Component::Serialize(j);
	j["mIsCollider"] = mIsCollider;
	j["mStaticObject"] = mStaticObject;
	j["mContactOffset"] = mContactOffset;
}

void Collider::Deserialize(const json& j)
{
	Component::Deserialize(j);
	mIsCollider = j.value("mIsCollider", true);
	mStaticObject = j.value("mStaticObject", true);
	mContactOffset = j.value("mContactOffset", 0.001f);
}

void Collider::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
	ImGui::Checkbox("Is Collider", &mIsCollider);
	ImGui::NewLine();
	ImGui::Checkbox("StaticObject", &mStaticObject);
	ImGui::NewLine();
	ImGui::SetNextItemWidth(50);
	ImGui::DragFloat("mContactOffset", &mContactOffset);
	ImGui::NewLine();

	ImGui::Separator();
}
