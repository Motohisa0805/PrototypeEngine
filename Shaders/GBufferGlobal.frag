

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

uniform sampler2DShadow uShadowMap;
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

void main()
{
	vec3 gbufferDiffuse = texture(uGDiffuse, fragTexCoord).xyz;
	vec3 gbufferNorm = texture(uGNormal, fragTexCoord).xyz;
	vec3 gbufferWorldPos = texture(uGWorldPos, fragTexCoord).xyz;
	
    vec4 gbufferPBR = texture(uGPBR,fragTexCoord);
    float metallic = gbufferPBR.r;
    float roughness = gbufferPBR.g;
    vec3 gbufferEmissive = texture(uGEmissive,fragTexCoord).rgb;

    if(length(gbufferNorm) < 0.01)
    {
        outColor = vec4(gbufferDiffuse + gbufferEmissive,1.0);
        return;
    }

    //=====================================
    //メタリックとラフネスによるディフューズ/スペキュラーの分離
    //=====================================
    //金属(Metallic = 1.0)は拡散反射(Diffuse)がほぼ0になり、ベースカラーがスペキュラー色になる
    vec3 realDiffuse = mix(gbufferDiffuse,vec3(0.0),metallic);
    vec3 realSpecular = mix(vec3(0.04),gbufferDiffuse,metallic);

    //ラフネス(0.0ツルツル～1.0:ザラザラ)をフォン鏡面反射指数に返還
    //Cook-Torrance BRDFに移行するまでの簡易変換
    float shininess = mix(256.0,2.0,roughness);
    
    // 法線正規化
	vec3 N = normalize(gbufferNorm);
	// ライトの方向（逆ベクトル）
	vec3 L = normalize(-uDirLight.mDirection);
	// カメラの方向
	vec3 V = normalize(uCameraPos - gbufferWorldPos);
	// -LとNから反射ベクトルを計算
	vec3 R = normalize(reflect(-L, N));

	// フォン反射を計算する
	vec3 Phong = uAmbientLight * uAmbientIntensity;
	float NdotL = dot(N, L);
	if (NdotL > 0)
	{
		vec3 Diffuse = uDirLight.mDiffuseColor * NdotL;

        //R(反射光)とV(視点)の向きが近いほど強く光る
        float RdotV = max(dot(R,V),0.0);
        //32.0はハイライトの硬さ
        vec3 Specular = uDirLight.mSpecColor * realSpecular * pow(RdotV,shininess);

        float shadow = 1.0; // デフォルトは「影なし = 100%ライトが届く」
        
        if (uEnableShadow) 
        {
            // フラグがtrueのときだけ影を計算
            vec2 randomRot = vec2(fract(sin(dot(fragTexCoord.xy ,vec2(12.9898,78.233))) * 43758.5453), 0.0);
            shadow = ComputeShadow_Poisson(vec4(gbufferWorldPos, 1.0), randomRot);
            shadow = clamp(shadow, 0.0, 1.0);
        }
        Phong += (Diffuse + Specular) * shadow;
	}


	// Phongスペキュラ計算
	Phong = clamp(Phong, 0.0, 1.0);

    vec3 finalColor = realDiffuse * Phong;
    //ポイントライトの処理
    for(int i = 0; i < uNumLights; ++i)
    {
        if(uLights[i].sType == 0)
        {
            //ライトの方向と距離
            vec3 lightDir = uLights[i].sPosition - gbufferWorldPos;
            float distance = length(lightDir);

            //ライトの有効範囲内にある場合のみ計算
            if(distance < uLights[i].sRange)
            {
                lightDir = normalize(lightDir);

                //距離減衰の計算
                float attenuation = 1.0 - (distance / uLights[i].sRange);
                attenuation = clamp(attenuation,0.0,1.0);

                //ディフューズ(拡散反射)の強度計算
                float nDotL = max(dot(N,lightDir),0.0);
                vec3 diffuse = realDiffuse * nDotL;

                //スペキュラーの計算
                vec3 R = normalize(reflect(-lightDir,N));
                float RdotV = max(dot(R,V),0.0);
                vec3 specular = realSpecular * pow(RdotV,shininess);

                //このライトの色
                vec3 lightContribution = uLights[i].sColor * (diffuse + specular) * nDotL * attenuation;

                //最終カラーに加算ブレンド
                finalColor += lightContribution;
            }
        }
        else if(uLights[i].sType == 1)
        {
            //ライトの方向と距離
            vec3 lightDir = uLights[i].sPosition - gbufferWorldPos;
            float distance = length(lightDir);

            if(distance < uLights[i].sRange)
            {
                // 正規化（ピクセルからライトへの向き）
                vec3 L = normalize(lightDir);
                // ライトが向いている方向（逆向きにして比較しやすくする）
                vec3 spotDir = normalize(-uLights[i].sDirection);

                //距離による減衰
                float distAtten = 1.0 - (distance / uLights[i].sRange);
                distAtten = clamp(distAtten, 0.0 , 1.0);

                //コーン(角度)による減衰
                //ライトの正面方向(spotDir)と、ピクセルへの方向(L)の内積 = cos(角度)
                float theta = dot(L,spotDir);

                // smoothstepを使って、外角(sAngles.y) ～ 内角(sAngles.x) の間を 0.0 ～ 1.0 に滑らかに補間
                // thetaが外角より小さければ0.0、内角より大きければ1.0になる
                float epsilon = uLights[i].sAngles.x - uLights[i].sAngles.y;
                float coneAtten = clamp((theta - uLights[i].sAngles.y) / epsilon,0.0,1.0);

                //最終的な光の強さは「距離減衰 × 角度減衰」
                float finalAtten = distAtten * coneAtten;

                if(finalAtten > 0.0)
                {
                    //ディフューズ(拡散反射)計算
                    float nDotL = max(dot(N,L),0.0);
                    vec3 diffuse = realDiffuse * nDotL;

                    //スペキュラーの計算
                    vec3 R = normalize(reflect(-L,N));
                    float RdotV = max(dot(R,V),0.0);
                    vec3 specular = realSpecular * pow(RdotV,shininess);

                    vec3 lightContribution = uLights[i].sColor * (diffuse + specular) * finalAtten;

                    //最終カラーに加算ブレンド
                    finalColor += lightContribution;
                }
            }
        }
    }

    finalColor += gbufferEmissive;

	// 最終的なライト情報を渡す (alpha = 1)
	outColor = vec4(finalColor,1.0);
}
