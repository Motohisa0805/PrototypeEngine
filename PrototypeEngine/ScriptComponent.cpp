#include "InGameProject/pch.h"
#include "ScriptComponent.h"

//===============================================================
//スクリプトの基本クラス
//ユーザーはこのクラス内の記述はしない設計
//===============================================================

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
	Debug::Log("ScriptComponent Awake");
}

void ScriptComponent::OnEnable()
{
	Debug::Log("ScriptComponent OnEnable");
}

void ScriptComponent::Start()
{
	Debug::Log("ScriptComponent Start");
}

void ScriptComponent::Update(float deltaTime)
{

}

void ScriptComponent::FixedUpdate(float deltaTime)
{

}

void ScriptComponent::OnCollisionEnter(ActorObject* target)
{

}

void ScriptComponent::OnCollisionStay(ActorObject* target)
{

}

void ScriptComponent::OnCollisionExit(ActorObject* target)
{

}

void ScriptComponent::OnDisable()
{
	Debug::Log("ScriptComponent OnDisable");
}

void ScriptComponent::Destroy()
{
	Debug::Log("ScriptComponent Destroyed");
}
