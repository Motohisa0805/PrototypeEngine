#pragma once
#include "Component.h"
#include "Shader.h"
#include "Actor.h"

//�����̋@�\
class DirectionalLightComponent : public Component
{
private:
	DirectionalLightData mDirectionalLight;
public:
	DirectionalLightComponent(ActorObject* owner);
	~DirectionalLightComponent();
	void OnUpdateWorldTransform()override;

	DirectionalLightData GetDirectionalLight() const { return mDirectionalLight; }  
};