#include "DirectionalLightComponent.h"

DirectionalLightComponent::DirectionalLightComponent(ActorObject* owner)
	: Component(owner)
	, mDirectionalLight()
{
    mDirectionalLight.mDirection = Vector3(0.0f, -0.707f, -0.707f);
    mDirectionalLight.mDiffuseColor = Vector3(0.78f, 0.88f, 1.0f);
    mDirectionalLight.mSpecColor = Vector3(0.8f, 0.8f, 0.8f);
}

DirectionalLightComponent::~DirectionalLightComponent()
{
}

void DirectionalLightComponent::OnUpdateWorldTransform()
{
    // ライトベクトルの基準軸（太陽が右から左に動く＝X軸方向）
    Vector3 baseDir = Vector3::UnitX;

    // 回転を適用してライトの最終的な方向を得る（ワールド空間）
    Vector3 lightDir = Vector3::Transform(baseDir, mOwner->GetRotation());
    //lightDir.Normalize();

    // ライト方向を保存
    mDirectionalLight.mDirection = lightDir;


    // ライトのY成分（太陽の高さ）を使って環境光を調整
    float y = Math::Clamp(lightDir.y, 0.0f, 1.0f);

    // Ambient（環境光）
    Vector3 dayAmbient = Vector3(0.7f, 0.7f, 0.6f);
    Vector3 nightAmbient = Vector3(0.05f, 0.05f, 0.1f);
    mDirectionalLight.mAmbientColor = Vector3::Lerp(dayAmbient, nightAmbient, y);

    // Diffuse（拡散光）
    Vector3 dayDiffuse = Vector3(1.0f, 0.95f, 0.8f);
    Vector3 nightDiffuse = Vector3(0.05f, 0.05f, 0.1f);
    mDirectionalLight.mDiffuseColor = Vector3::Lerp(dayDiffuse, nightDiffuse, y);

    // Specular（鏡面反射）
    Vector3 daySpecular = Vector3(1.0f, 1.0f, 1.0f);
    Vector3 nightSpecular = Vector3(0.0f, 0.0f, 0.0f);
    mDirectionalLight.mSpecColor = Vector3::Lerp(daySpecular, nightSpecular, y);



    mDirectionalLight.mPosition = mOwner->GetPosition();
    EngineWindow::GetRenderer()->SetDirectionalLight(mDirectionalLight);
}
