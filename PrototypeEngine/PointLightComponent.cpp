#include "PointLightComponent.h"
#include "Shader.h"
#include "EngineWindow.h"
#include "BaseScene.h"
#include "Renderer.h"
#include "Mesh.h"
#include "VertexArray.h"
#include "Actor.h"

PointLightComponent::PointLightComponent(Entity* owner)
	:Component(owner)
	,mColor(Vector3())
	,mRange(1.0f)
	,mIntensity(1.0f)
{
	mName = "PointLight";

	EngineWindow::GetRenderer()->AddPointLight(this);

	mHeaderColor = Vector4(0.4f, 0.8f, 0.8f, 1.0f);
	mHeaderHoveredColor = Vector4(0.3f, 0.6f, 0.6f, 1.0f);
	mHeaderActiveColor = Vector4(0.4f, 0.8f, 0.8f, 1.0f);
}

PointLightComponent::~PointLightComponent()
{
	EngineWindow::GetRenderer()->RemovePointLight(this);
}

void PointLightComponent::Draw(Shader* shader, Mesh* mesh)
{
	/*
	for (unsigned int i = 0; i < mesh->GetVertexArrays().size(); i++) 
	{
		// この関数に入るにあたり、シェーダーがアクティブであり、
		// 球メッシュもアクティブであると仮定します。
		// ワールド変換は外半径にスケーリングされ
		// （メッシュ半径で割ったもの）、ワールド位置に配置されます。
		Matrix4 scale = Matrix4::CreateScale(mOwner->GetBaseTransform()->GetLocalScale() * mOuterRadius / mesh->GetRadiusArray()[i]);
		Matrix4 trans = Matrix4::CreateTranslation(mOwner->GetBaseTransform()->GetLocalPosition());
		Matrix4 worldTransform = scale * trans;
		shader->SetMatrixUniform("uWorldTransform", worldTransform);
		//ポイントライトシェーダー定数を設定する
		shader->SetVectorUniform("uPointLight.mWorldPos", mOwner->GetBaseTransform()->GetLocalPosition());
		shader->SetVectorUniform("uPointLight.mDiffuseColor", mDiffuseColor);
		shader->SetFloatUniform("uPointLight.mInnerRadius", mInnerRadius);
		shader->SetFloatUniform("uPointLight.mOuterRadius", mOuterRadius);

		// 球を描画
		glDrawElements(GL_TRIANGLES, mesh->GetVertexArrays()[i]->GetNumIndices(),
			GL_UNSIGNED_INT, nullptr);
	}
	*/
}

void PointLightComponent::Serialize(json& j) const
{
	Component::Serialize(j);
	j["Color"] = {mColor.x,mColor.y,mColor.z };
	j["Range"] = mRange;
	j["Intensity"] = mIntensity;
}

void PointLightComponent::Deserialize(const json& j)
{
	Component::Deserialize(j);
	if (j.contains("Color")) {
		auto colorArray = j["Color"];
		mColor.Set(colorArray[0], colorArray[1], colorArray[2]);
	}
	if (j.contains("Range")) {
		auto range = j.at("Range").get<float>();
		mRange = range;
	}
	if (j.contains("Intensity")) {
		auto intensity = j.at("Intensity").get<float>();
		mIntensity = intensity;
	}
}

void PointLightComponent::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
	ImGui::PushID(this);

	ImGui::Separator();

	ImGui::Text("Light Color");
	ImGui::SetNextItemWidth(200);
	if (ImGui::ColorEdit3("##lightColor", &mColor.x)) {
		mActor->GetTransform()->SetDirty();
	}

	ImGui::Text("Intensity");
	if (ImGui::SliderFloat("##intensity", &mIntensity, 0.0f, 5.0f)) {
		mActor->GetTransform()->SetDirty();
	}

	ImGui::Text("Light Range");
	if (ImGui::DragFloat("##lightRange", &mRange)) {
		mActor->GetTransform()->SetDirty();
	}

	ImGui::Separator();

	ImGui::PopID();
}

Component* PointLightComponent::Clone(Entity* newOwner) const
{
	PointLightComponent* clone = new PointLightComponent(newOwner);
	clone->mColor = this->mColor;
	clone->mRange = this->mRange;
	clone->mIntensity = this->mIntensity;
	return clone;
}
