#include "Actor.h"

ActorObject::ActorObject()
	: Transform()
	, mState(EActive)
	, mActorTag(ActorTag::None)
	, mGame(GameApp::GetActiveScene())
	, mCollider(nullptr)
	, mRigidbody(nullptr)
{
	mGame->AddActor(this);
}

ActorObject::~ActorObject()
{
	mGame->RemoveActor(this);
	// Need to delete components
	// Because ~Component calls RemoveComponent, need a different style loop
	while (!mComponents.empty())
	{
		delete mComponents.back();
	}
	mComponents.clear();
}

void ActorObject::FixedUpdate(float deltaTime)
{
	if (mState == EActive)
	{
		FixedUpdateComponents(deltaTime);
		FixedUpdateActor(deltaTime);
		ComputeWorldTransform(NULL);
	}
}

void ActorObject::FixedUpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
	{
		comp->FixedUpdate(deltaTime);
	}
}

void ActorObject::FixedUpdateActor(float deltaTime)
{
}

void ActorObject::Update(float deltaTime)
{
	if (mState == EActive)
	{
		//ComputeLocalTransform();
		UpdateComponents(deltaTime);
		UpdateActor(deltaTime);
		ComputeWorldTransform(NULL);
	}
}

void ActorObject::UpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
	{
		comp->Update(deltaTime);
	}
}

void ActorObject::UpdateActor(float deltaTime)
{
}

void ActorObject::ProcessInput(const struct InputState& keyState)
{
	if (mState == EActive)
	{
		// First process input for components
		for (auto comp : mComponents)
		{
			comp->ProcessInput(keyState);
		}

		ActorInput(keyState);
	}
}

void ActorObject::ActorInput(const struct InputState& keyState)
{
}
