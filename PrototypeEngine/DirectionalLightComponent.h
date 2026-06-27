#pragma once
#include "Component.h"
#include "Shader.h"
#include "Actor.h"
#include "Renderer.h"

//環境光の機能
//光を扱ったコンポーネント
class DirectionalLightComponent : public Component
{
private:	
	//環境光の変数
	DirectionalLightData	mDirectionalLight;
public:
	DirectionalLightComponent(Entity* owner);
	~DirectionalLightComponent();
	void OnUpdateWorldTransform()override;

	DirectionalLightData	GetDirectionalLight() const { return mDirectionalLight; } 

	void					Serialize(json& j) const override;
	void					Deserialize(const json& j)override;

	void					DrawCustomGUI(const std::vector<PropertyInfo>& properties)override;

	Component*				Clone(Entity* newOwner) const override;
};