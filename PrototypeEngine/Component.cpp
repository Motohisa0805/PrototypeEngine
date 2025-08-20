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