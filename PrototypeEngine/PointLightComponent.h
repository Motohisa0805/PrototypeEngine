#pragma once
#include "Math.h"
#include "Component.h"
#include "Shader.h"
#include "Actor.h"
#include "Renderer.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/

//書籍部分
//ポイントライトの処理を行うクラス
class PointLightComponent : public Component
{
private:
	// Diffuse color
	Vector3 mColor;
	// Radius of light
	float	mRange;

	float	mIntensity;
public:
			PointLightComponent(Entity* owner);
			~PointLightComponent();

	// このポイントライトを描画
	void					Draw(class Shader* shader, class Mesh* mesh);

	Vector3					GetColor() { return mColor * mIntensity; }

	float					GetRange() { return mRange; }

	void					Serialize(json& j) const override;
	void					Deserialize(const json& j)override;

	void					DrawCustomGUI(const std::vector<PropertyInfo>& properties)override;

	Component*				Clone(Entity* newOwner) const override;
};

