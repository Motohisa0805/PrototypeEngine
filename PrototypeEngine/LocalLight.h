#pragma once
#include "Math.h"
#include "Component.h"
#include "Shader.h"
#include "Actor.h"
#include "Renderer.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/

//ポイントライトの処理を行うクラス
class LocalLight : public Component
{
public:
	enum LightType {
		Point = 0,
		Spot,
		Area,
		Count
	};
private:
	LightType				mLightType;

	//ポイントライトの変数
	// Diffuse color
	Vector3 mColor;
	// Radius of light
	float	mRange;

	float	mIntensity;	
	//スポットライトの変数
	float	mAngles;
	//エリアライトの変数
public:
			LocalLight(Entity* owner);
			~LocalLight();

	void					SetLightComponentName(LightType type);
	string					GetLightComponentName(LightType type);
	
	LightType				GetLightType() { return mLightType; }

	Vector3					GetColor() { return mColor * mIntensity; }

	float					GetRange() { return mRange; }

	Vector2					GetAngles();

	void					Serialize(json& j) const override;
	void					Deserialize(const json& j)override;

	void					DrawCustomGUI(const std::vector<PropertyInfo>& properties)override;

	Component*				Clone(Entity* newOwner) const override;
};

