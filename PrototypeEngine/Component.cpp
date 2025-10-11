#include "Component.h"
#include "Actor.h"

Component::Component(ActorObject* owner, int updateOrder)
	:mOwner(owner)
	, mUpdateOrder(updateOrder)
	, mGame(owner->GetGame())
	, mName("Component")
{
	mOwner->SetDirty();
	// Actorのコンポーネントベクトルに追加する
	//mOwner->AddComponent(this);
}

Component::~Component()
{
	mOwner->RemoveComponent(this);
}

void Component::FixedUpdate(float deltaTime)
{
}

void Component::Update(float deltaTime)
{
}

void Component::Serialize(json& j) const
{
	j["Type"] = mName;
	j["UpdateOrder"] = mUpdateOrder;
}

void Component::Deserialize(const json& j)
{
	mName = j.at("Type").get<string>();
	mUpdateOrder = j.at("UpdateOrder").get<int>();
}
