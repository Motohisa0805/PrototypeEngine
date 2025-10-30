#include "GameScripts/pch.h"
#include "ScriptComponent.h"

ScriptComponent::ScriptComponent(ActorObject* owner)
	: Component(owner)
{
	//デフォルトでコンポーネントの名前を「ScriptComponent」と設定
	mName = "ScriptComponent";
}

ScriptComponent::~ScriptComponent()
{
}

bool ScriptComponent::HasStarted() const
{
	return mHasStarted;
}

void ScriptComponent::SetStarted(bool started)
{
	mHasStarted = started;
}

void ScriptComponent::Awake()
{
}

void ScriptComponent::Start()
{
}

void ScriptComponent::Update(float deltaTime)
{
}

void ScriptComponent::FixedUpdate(float deltaTime)
{
}
