#pragma once
#include "Component.h"
#include "Shader.h"
#include "Actor.h"
#include "Renderer.h"

//ŠÂ‹«Œõ‚Ì‹@”\
class DirectionalLightComponent : public Component
{
private:
	DirectionalLightData mDirectionalLight;
public:
	DirectionalLightComponent(Entity* owner);
	~DirectionalLightComponent();
	void OnUpdateWorldTransform()override;

	DirectionalLightData GetDirectionalLight() const { return mDirectionalLight; } 

	void			Serialize(json& j) const override;
	void			Deserialize(const json& j)override;

	void			DrawCustomGUI(const std::vector<PropertyInfo>& properties)override;

	Component*		Clone(ActorObject* newOwner) const override;
};