#include "Component.h"
#include "Actor.h"

Component::Component(ActorObject* owner, int updateOrder)
	:mOwner(owner)
	, mUpdateOrder(updateOrder)
	, mGame(owner->GetGame())
{
	// Actorのコンポーネントベクトルに追加する
	mOwner->AddComponent(this);
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
	j["updateOrder"] = mUpdateOrder;
}

void Component::Deserialize(const json& j)
{
	mUpdateOrder = j.at("updateOrder").get<int>();
}

Component* Component::CreateComponent(const std::string& type, ActorObject* owner)
{
	return nullptr;
}