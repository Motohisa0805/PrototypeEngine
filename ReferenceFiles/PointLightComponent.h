#pragma once
#include "Math.h"
#include "Component.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//書籍部分
//ポイントライトの処理を行うクラス
class PointLightComponent : public Component
{
public:
			PointLightComponent(class ActorObject* owner);
			~PointLightComponent();

	// このポイントライトを描画
	void	Draw(class Shader* shader, class Mesh* mesh);

	// Diffuse color
	Vector3 mDiffuseColor;
	// Radius of light
	float	mInnerRadius;
	float	mOuterRadius;
};

