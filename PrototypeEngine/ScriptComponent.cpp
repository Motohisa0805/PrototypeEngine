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

}

void ScriptComponent::OnEnable()
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

void ScriptComponent::OnDisable()
{

}

void ScriptComponent::Destroy()
{

}
