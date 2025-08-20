#pragma once
#include "Actor.h"
#include "DirectionalLightComponent.h"

//環境光のオブジェクト
//Unityと同じように回転量で明るさが変わる
class DirectionalLightActor : public ActorObject
{
private:
	DirectionalLightComponent* mDirectionalLightComp;
public:
	DirectionalLightActor();
	~DirectionalLightActor();

	void ActorInput(const struct InputState& keyState)override;
};
