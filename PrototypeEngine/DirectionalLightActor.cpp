#include "DirectionalLightActor.h"

DirectionalLightActor::DirectionalLightActor()
	: ActorObject()
	, mDirectionalLightComp(nullptr)
{
	mDirectionalLightComp = new DirectionalLightComponent(this);
	Quaternion rot = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, -45.0f); // 90度：X→Yに向く → Y成分 = 1（昼！）
	SetLocalRotation(rot);
}

DirectionalLightActor::~DirectionalLightActor()
{
}

void DirectionalLightActor::ActorInput(const InputState& keyState)
{
	if (keyState.Keyboard.GetKey(KEY_Q))
	{
		// Rotate left
		Quaternion rot = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, -0.1f);
		SetLocalRotation(GetLocalRotation() * rot);
	}
	else if (keyState.Keyboard.GetKey(KEY_E))
	{
		// Rotate right
		Quaternion rot = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, 0.1f);
		SetLocalRotation(GetLocalRotation() * rot);
	}
}
