

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

float ComputeShadow_Poisson(vec4 worldPos, vec2 randomRot)
{
    vec4 lightSpacePos = worldPos * uLightViewProj;
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z < 0.0 || projCoords.z > 1.0)
    {
        return 1.0;
    }

    float shadow = 0.0;
    float bias = 0.005;
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

//法線分布関数
float DistributionGGX(vec3 N,vec3 H,float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N,H),0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / max(denom,0.0000001);
}
//幾何減衰関数(Geometry Function - Schlick-GGX)
float GeometrySchlickGGX(float NdotV,float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / max(denom,0.0000001);
}

//Smith方による幾何減衰(視線方向と光源方向の両方を考慮)
float GeometrySmith(vec3 N,vec3 V,vec3 L,float roughness)
{
    float NdotV = max(dot(N,V),0.0);
    float NdotL = max(dot(N,L),0.0);
    float ggx2 = GeometrySchlickGGX(NdotV,roughness);
    float ggx1 = GeometrySchlickGGX(NdotL,roughness);

    return ggx1 * ggx2;
}

//フレネル方程式
//F0 = 垂直に入射した時の基本反射率
vec3 FresnelSchlick(float cosTheta,vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta,0.0,1.0),5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta,vec3 F0,float roughness)
{
    return F0 + (max(vec3(1.0 - roughness),F0) - F0) * pow(clamp(1.0 - cosTheta,0.0,1.0),5.0);
}

//1つのライトに対するPBRライティング計算をまとめた関数
vec3 CalculatePBRLight(vec3 lightDir,vec3 lightColor,vec3 N,vec3 V,vec3 albedo,vec3 F0,float roughness)
{
    vec3 H = normalize(V + lightDir);
    float NdotL = max(dot(N,lightDir),0.0);
    float NdotV = max(dot(N,V),0.0);

    if(NdotL <= 0.0)return vec3(0.0);

    //Cook-Torrance BRDFの各項を計算
    float NDF = DistributionGGX(N,H,roughness);
    float G   = GeometrySmith(N,V,lightDir,roughness);
    vec3  F   = FresnelSchlick(max(dot(H,V),0.0),F0);

    //鏡面反射(Specular)の割合 = フレネル値 F
    vec3 kS = F;
    //拡散反射(Diffuse)の割合 = 光の全エネルギーから鏡面反射を引いた残り
    vec3 kD = vec3(1.0) - kS;

    //スペキュラー計算式:(D * G * F)/(4 * NdotV * NdotL)
    vec3 numerator      = NDF * G * F;
    float denominator   = 4.0 * NdotV * NdotL;
    vec3 specular       = numerator / max(denominator,0.0001);

    //出力 = (拡散反射 + 鏡面反射) * 光の色 * 入射角減衰(NdotL)
    return (kD * albedo / PI + specular) * lightColor * NdotL;
}

void main()
{
	vec3 gbufferDiffuse     = texture(uGDiffuse, fragTexCoord).xyz;
	vec3 gbufferNorm        = texture(uGNormal, fragTexCoord).xyz;
	vec3 gbufferWorldPos    = texture(uGWorldPos, fragTexCoord).xyz;
    vec4 gbufferPBR         = texture(uGPBR,fragTexCoord).rgba;
    vec3 gbufferSpecular    = gbufferPBR.rgb;
    float roughness         = gbufferPBR.a;
    vec3 gbufferEmissive    = texture(uGEmissive,fragTexCoord).rgb;

    if(length(gbufferNorm) < 0.01)
    {
        outColor = vec4(gbufferDiffuse + gbufferEmissive,1.0);
        return;
    }

    //PBR計算で扱いやすいように安全なラフネスにクランプ
    roughness = clamp(roughness,0.04,1.0);
    
    // 法線正規化
	vec3 N = normalize(gbufferNorm);
	// カメラの方向
	vec3 V = normalize(uCameraPos - gbufferWorldPos);

    float NdotV = max(dot(N,V),0.0);
    vec3 F_ambient = FresnelSchlickRoughness(NdotV,gbufferSpecular,roughness);

    vec3 kS = F_ambient;
    vec3 kD = vec3(1.0) - kS;

    //環境光のフレネル反射(視界角度での強弱)を計算
    vec3 ambientDiffuse = kD * uAmbientLight * uAmbientIntensity * gbufferDiffuse;
    
    //環境光(Ambient / IBL近似)
    vec3 envReflect = reflect(-V,N);
    vec3 envColor = texture(uSkybox,envReflect,roughness * 5.0).rgb;

	vec3 ambientSpecular = envColor * F_ambient;

    vec3 finalColor = ambientDiffuse + ambientSpecular;


    //ディレクショナルライト(平行光源)
    vec3 lightDir = normalize(-uDirLight.mDirection);
    vec3 dirLightColor = uDirLight.mDiffuseColor + uDirLight.mSpecColor;//カラー/強度の統合

    vec3 directLighting = CalculatePBRLight(lightDir,dirLightColor,N,V,gbufferDiffuse,gbufferSpecular,roughness);
    
    //シャドウ計算
    float shadow = 1.0; // デフォルトは「影なし = 100%ライトが届く」
    if (uEnableShadow) 
    {
        // フラグがtrueのときだけ影を計算
        vec2 randomRot = vec2(fract(sin(dot(fragTexCoord.xy ,vec2(12.9898,78.233))) * 43758.5453), 0.0);
        shadow = ComputeShadow_Poisson(vec4(gbufferWorldPos, 1.0), randomRot);
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

                vec3 lightResult = CalculatePBRLight(pointLightDir,uLights[i].sColor,N,V,gbufferDiffuse,gbufferSpecular,roughness);

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
                    vec3 lightResult = CalculatePBRLight(normalSpotLight,uLights[i].sColor,N,V,gbufferDiffuse,gbufferSpecular,roughness);
                    //最終カラーに加算ブレンド
                    finalColor += lightResult * finalAtten;
                }
            }
        }
    }

    finalColor += gbufferEmissive;

	// 最終的なライト情報を渡す (alpha = 1)
	outColor = vec4(finalColor,1.0);
}
