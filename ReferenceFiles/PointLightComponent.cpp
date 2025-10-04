#include "PointLightComponent.h"
#include "Shader.h"
#include "GameWinMain.h"
#include "BaseScene.h"
#include "Renderer.h"
#include "Mesh.h"
#include "VertexArray.h"
#include "Actor.h"

PointLightComponent::PointLightComponent(ActorObject* owner)
	:Component(owner)
{
	owner->GetGame()->GetWinMain()->GetRenderer()->AddPointLight(this);
}

PointLightComponent::~PointLightComponent()
{
	GetOwner()->GetGame()->GetWinMain()->GetRenderer()->RemovePointLight(this);
}

void PointLightComponent::Draw(Shader* shader, Mesh* mesh)
{
	for (unsigned int i = 0; i < mesh->GetVertexArrays().size(); i++) 
	{
		// この関数に入るにあたり、シェーダーがアクティブであり、
		// 球メッシュもアクティブであると仮定します。
		// ワールド変換は外半径にスケーリングされ
		// （メッシュ半径で割ったもの）、ワールド位置に配置されます。
		Matrix4 scale = Matrix4::CreateScale(mOwner->GetLocalScale() * mOuterRadius / mesh->GetRadiusArray()[i]);
		Matrix4 trans = Matrix4::CreateTranslation(mOwner->GetLocalPosition());
		Matrix4 worldTransform = scale * trans;
		shader->SetMatrixUniform("uWorldTransform", worldTransform);
		//ポイントライトシェーダー定数を設定する
		shader->SetVectorUniform("uPointLight.mWorldPos", mOwner->GetLocalPosition());
		shader->SetVectorUniform("uPointLight.mDiffuseColor", mDiffuseColor);
		shader->SetFloatUniform("uPointLight.mInnerRadius", mInnerRadius);
		shader->SetFloatUniform("uPointLight.mOuterRadius", mOuterRadius);

		// 球を描画
		glDrawElements(GL_TRIANGLES, mesh->GetVertexArrays()[i]->GetNumIndices(),
			GL_UNSIGNED_INT, nullptr);
	}
}