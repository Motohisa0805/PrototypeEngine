#include "TargetComponent.h"
#include "Actor.h"
#include "PhysWorld.h"
#include "Math.h"

TargetComponent::TargetComponent(ActorObject* owner)
	: Component(owner)
{
	mName = "TargetComponent";
}

TargetComponent::~TargetComponent()
{
}
