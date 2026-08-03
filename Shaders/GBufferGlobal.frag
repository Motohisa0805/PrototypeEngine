

// Request GLSL 3.3
#version 330

// 頂点シェーダーからの入力
// 画像のUV座標
in vec2 fragTexCoord;

// カラーテクスチャ出力
layout(location = 0) out vec4 outColor;

// G-Bufferテクスチャ
uniform sampler2D uGDiffuse;
uniform sampler2D uGNormal;
uniform sampler2D uGWorldPos;
//PBR情報||エミッション情報
uniform sampler2D uGPBR;
uniform sampler2D uGEmissive;
//スカイボックス画像
uniform samplerCube uSkybox;
//シャドウマッピング
uniform sampler2DShadow uShadowMap;

// ディレクショナルライトの構造体
struct DirectionalLight
{
	// Direction of light
	vec3 mDirection;
	// Diffuse color
	vec3 mDiffuseColor;
	// Specular color
	vec3 mSpecColor;
};

// ライト情報
// カメラ座標 (ワールド空間)
uniform vec3 uCameraPos;
// 環境ライト
uniform vec3 uAmbientLight;
//環境光の強さ
uniform float uAmbientIntensity;
// 光の方向
uniform DirectionalLight uDirLight;

uniform mat4 uLightViewProj;

uniform bool uEnableShadow;

struct LightData{
    int     sType;      //0:Point、1:Spot、2:Area
    vec3    sPosition;
    vec3    sDirection;
    vec3    sColor;
    float   sRange;
    vec2    sAngles;    //スポットライト用の角度
};

//ポイントライトの変数
const int MAX_POINT_LIGHTS = 20;

uniform int uNumLights;

uniform LightData uLights[MAX_POINT_LIGHTS];


// 事前に固定Poissonオフセット（正規化）を定義
const vec2 poissonDisk[16] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2(0.94558609, -0.76890725),
    vec2(-0.09418410, -0.92938870),
    vec2(0.34495938,  0.29387760),
    vec2(-0.91588581,  0.45771432),
    vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543,  0.27676845),
    vec2(0.97484398,   0.75648379),
    vec2(0.44323325,  -0.97511554),
    vec2(0.53742981,  -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2(0.79197514,   0.19090188),
    vec2(-0.24188840,  0.99706507),
    vec2(-0.81409955,  0.91437590),
    vec2(0.19984126,  -0.78641367),
    vec2(0.14383161,  -0.14100790)
);

float ComputeShadow_Poisson(vec4 worldPos,vec3 normal,vec3 lightDir, vec2 randomRot)
{
    //法線バイアスの適用
    float normalBias = 0.005;
    float dotNL = max(dot(normal,lightDir), 0.0);
    vec3 biasedWorldPos = worldPos.xyz + (normal * normalBias * (1.0 - dotNL));

    vec4 lightSpacePos = vec4(biasedWorldPos, 1.0) * uLightViewProj;
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z < 0.0 || projCoords.z > 1.0)
    {
        return 1.0;
    }

    float shadow = 0.0;
    float bias = 0.001;
    float texelSize = 1.0 / 1024.0;
    float radius = 2.5; // 半径調整（見た目のソフトさ）

    // ランダム回転マトリクス
    mat2 rot = mat2(
        cos(randomRot.x), -sin(randomRot.x),
        sin(randomRot.x),  cos(randomRot.x)
    );

    for (int i = 0; i < 16; ++i)
    {
        vec2 offset = rot * poissonDisk[i] * texelSize * radius;
        shadow += texture(uShadowMap, vec3(projCoords.xy + offset, projCoords.z - bias));
    }

    shadow /= 16.0;
    return shadow;
}

const float PI = 3.14159265359;

//=================================
//PBR用ヘルパー関数群(Cook-Torrance BRDF)
//=================================

float saturate(float f)
{
    return clamp(f,0.0,1.0);
}

//法線分布関数
float DistributionGGX(vec3 N,vec3 H,float roughness)
{
    float roughness2 = roughness * roughness;
    float dotNH = saturate(dot(N, H));
    float a = (1.0 - (1.0 - roughness2) * dotNH * dotNH);
    return roughness2 * PI / (a * a);
}
//幾何減衰関数(Geometry Function - Schlick-GGX)
float GeometrySchlickGGX(vec3 normal,vec3 viewDir,float roughness)
{   
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float dotNV = saturate(dot(normal,viewDir));
    float num = dotNV;
    float denom = dotNV * (1.0 - k) + k;

    return num / denom;
}

//Smith方による幾何減衰(視線方向と光源方向の両方を考慮)
float GeometrySmith(vec3 N,vec3 V,vec3 L,float roughness)
{
    float ggx1 = GeometrySchlickGGX(N,V,roughness);
    float ggx2 = GeometrySchlickGGX(N,L,roughness);

    return ggx1 * ggx2;
}

//フレネル方程式
//f0 = 垂直に入射した時の基本反射率
vec3 FresnelSchlick(float cosTheta,vec3 f0)
{
    return f0 + (1.0 - f0) * pow(1.0 - max(cosTheta,0.0),5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta,vec3 f0,float roughness)
{
    return f0 + (max(vec3(1.0 - roughness),f0) - f0) * pow(clamp(1.0 - cosTheta,0.0,1.0),5.0);
}

//1つのライトに対するPBRライティング計算をまとめた関数
vec3 CalculatePBRLight(vec3 lightDir,vec3 lightColor,vec3 normal,vec3 viewDir ,vec3 albedo,vec3 f0,float roughness)
{
    vec3 halfDir  = normalize(viewDir  + lightDir);
    float dotNV = saturate(dot(normal,viewDir ));
    float dotNL = saturate(dot(normal,lightDir));
    float dotVH = saturate(dot(viewDir , halfDir));


    //Cook-Torrance BRDFの各項を計算
    float d = DistributionGGX(normal,halfDir ,roughness);
    float g = GeometrySmith(normal,viewDir ,lightDir,roughness);
    vec3  f = FresnelSchlick(dotVH,f0);

    //鏡面反射(Specular)の割合 = フレネル値 f
    vec3 kS = f;
    //拡散反射(Diffuse)の割合 = 光の全エネルギーから鏡面反射を引いた残り
    vec3 kD = vec3(1.0) - kS;

    //スペキュラー計算式:(D * g * f)/(4 * dotNV * dotNL)
    vec3 numerator      = d * g * f;
    float denominator   = 4.0 * dotNV * dotNL + 0.0001;
    vec3 specular       = numerator / denominator;

    //出力 = (拡散反射 + 鏡面反射) * 光の色 * 入射角減衰(dotNL)
    return (kD * albedo / PI + specular) * lightColor * dotNL;
}

float GetDistanceAttenuation(float distanceSqr,float range)
{
    float rangeSqr = range * range;
    //有効範囲でスムーズに0にするウィンドウ関数
    float factor = distanceSqr / rangeSqr;
    float smoothFactor = saturate(1.0 - factor * factor);
    smoothFactor = smoothFactor * smoothFactor;
    //逆二乗則
    return smoothFactor / max(distanceSqr,0.0001);
}

void main()
{
	vec3 gbufferDiffuse     = texture(uGDiffuse, fragTexCoord).xyz;
	vec3 gbufferNorm        = texture(uGNormal, fragTexCoord).xyz;
	vec3 gbufferWorldPos    = texture(uGWorldPos, fragTexCoord).xyz;
    vec4 gbufferPBR         = texture(uGPBR,fragTexCoord).rgba;
    vec3 gbufferEmissive    = texture(uGEmissive,fragTexCoord).rgb;

    if(length(gbufferNorm) < 0.01)
    {
        outColor = vec4(gbufferDiffuse + gbufferEmissive,1.0);
        return;
    }

    //PBR計算で扱いやすいように安全なラフネスにクランプ
    float metallic = saturate(length(gbufferPBR.r));
    float roughness = saturate(gbufferPBR.a);
    //非金属の基本反射率は0.04、金属はアルベドカラーをそのまま反射率として扱う
    vec3 f0 = mix(vec3(0.04),gbufferDiffuse,metallic);
    //金属は拡散反射(ディフューズ)を持たないため、メタリック値に応じて黒に近づける
    vec3 albedo = gbufferDiffuse * (1.0 - metallic);
    
    // 法線正規化
	vec3 normal = normalize(gbufferNorm);
	// カメラの方向
	vec3 viewDir = normalize(uCameraPos - gbufferWorldPos);
    float dotNV = max(dot(normal,viewDir),0.001);

    //IBL / 環境光のフレネル反射を計算
    vec3 f_ambient = FresnelSchlickRoughness(dotNV,f0,roughness);
    //環境光のフレネル反射(視界角度での強弱)を計算
    vec3 ambientDiffuse = uAmbientLight * uAmbientIntensity * albedo;
    
    vec3 reflectDir = reflect(-viewDir,normal);
    float maxLod = 6.0;
    vec3 envColor = textureLod(uSkybox,reflectDir,roughness * maxLod).rgb;

    vec3 skyReflectColor = envColor + (uAmbientLight * uAmbientIntensity * 0.5);

	vec3 ambientSpecular = skyReflectColor * f_ambient;

    vec3 finalColor = ambientDiffuse + ambientSpecular;


    //ディレクショナルライト(平行光源)
    vec3 lightDir = normalize(-uDirLight.mDirection);
    vec3 directLighting = CalculatePBRLight(lightDir,uDirLight.mDiffuseColor,normal,viewDir,albedo,f0,roughness);
    
    //シャドウ計算
    float shadow = 1.0; // デフォルトは「影なし = 100%ライトが届く」
    if (uEnableShadow) 
    {
        // フラグがtrueのときだけ影を計算
        vec2 randomRot = vec2(fract(sin(dot(fragTexCoord.xy ,vec2(12.9898,78.233))) * 43758.5453), 0.0);
        shadow = ComputeShadow_Poisson(vec4(gbufferWorldPos, 1.0),normal,lightDir, randomRot);
        shadow = clamp(shadow, 0.0, 1.0);
    }

    finalColor += directLighting * shadow;

    //ポイントライトの処理
    for(int i = 0; i < uNumLights; ++i)
    {
        if(uLights[i].sType == 0)
        {
            //ライトの方向と距離
            vec3 pointLightDir = uLights[i].sPosition - gbufferWorldPos;
            float distance = max(length(pointLightDir),0.0001);

            //ライトの有効範囲内にある場合のみ計算
            if(distance < uLights[i].sRange)
            {
                pointLightDir = normalize(pointLightDir);
                //距離減衰の計算
                float attenuation = 1.0 - (distance / uLights[i].sRange);
                attenuation = clamp(attenuation,0.0,1.0);

                vec3 lightResult = CalculatePBRLight(pointLightDir,uLights[i].sColor,normal,viewDir,albedo,f0,roughness);

                //最終カラーに加算ブレンド
                finalColor += lightResult * attenuation;
            }
        }
        else if(uLights[i].sType == 1)
        {
            //ライトの方向と距離
            vec3 spotLightDir = uLights[i].sPosition - gbufferWorldPos;
            float distance = length(spotLightDir);

            if(distance < uLights[i].sRange)
            {
                // 正規化（ピクセルからライトへの向き）
                vec3 normalSpotLight = normalize(spotLightDir);
                // ライトが向いている方向（逆向きにして比較しやすくする）
                vec3 spotDir = normalize(-uLights[i].sDirection);

                //距離による減衰
                float distAtten = 1.0 - (distance / uLights[i].sRange);
                distAtten = clamp(distAtten, 0.0 , 1.0);

                //コーン(角度)による減衰
                //ライトの正面方向(spotDir)と、ピクセルへの方向(normalSpotLight)の内積 = cos(角度)
                float theta = dot(normalSpotLight,spotDir);

                // smoothstepを使って、外角(sAngles.y) ～ 内角(sAngles.x) の間を 0.0 ～ 1.0 に滑らかに補間
                // thetaが外角より小さければ0.0、内角より大きければ1.0になる
                float epsilon = uLights[i].sAngles.x - uLights[i].sAngles.y;
                float coneAtten = clamp((theta - uLights[i].sAngles.y) / epsilon,0.0,1.0);

                //最終的な光の強さは「距離減衰 × 角度減衰」
                float finalAtten = distAtten * coneAtten;

                if(finalAtten > 0.0)
                {
                    vec3 lightResult = CalculatePBRLight(normalSpotLight,uLights[i].sColor,normal,viewDir,albedo,f0,roughness);
                    //最終カラーに加算ブレンド
                    finalColor += lightResult * finalAtten;
                }
            }
        }
    }
    //一時的に無効
    //finalColor += gbufferEmissive;

	// 最終的なライト情報を渡す (alpha = 1)
	outColor = vec4(finalColor,1.0);
}
