

// Request GLSL 3.3
#version 330

// Inputs from vertex shader
// Tex coord
in vec2 fragTexCoord;

// This corresponds to the output color to the color buffer
layout(location = 0) out vec4 outColor;

// Different textures from G-buffer
uniform sampler2D uGDiffuse;
uniform sampler2D uGNormal;
uniform sampler2D uGWorldPos;

// Create a struct for directional light
struct DirectionalLight
{
	// Direction of light
	vec3 mDirection;
	// Diffuse color
	vec3 mDiffuseColor;
	// Specular color
	vec3 mSpecColor;
};

// Uniforms for lighting
// Camera position (in world space)
uniform vec3 uCameraPos;
// Ambient light level
uniform vec3 uAmbientLight;
// Directional Light
uniform DirectionalLight uDirLight;

uniform sampler2DShadow uShadowMap;
uniform mat4 uLightViewProj;

uniform bool uEnableShadow;

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
	// Surface normal
	vec3 N = normalize(gbufferNorm);
	// Vector from surface to light
	vec3 L = normalize(-uDirLight.mDirection);
	// Vector from surface to camera
	vec3 V = normalize(uCameraPos - gbufferWorldPos);
	// Reflection of -L about N
	vec3 R = normalize(reflect(-L, N));

	// Compute phong reflection
	vec3 Phong = uAmbientLight;
	float NdotL = dot(N, L);
	if (NdotL > 0)
	{
		vec3 Diffuse = uDirLight.mDiffuseColor * NdotL;

        float shadow = 1.0; // デフォルトは「影なし = 100%ライトが届く」
        
        if (uEnableShadow) 
        {
            // フラグがtrueのときだけ影を計算
            vec2 randomRot = vec2(fract(sin(dot(fragTexCoord.xy ,vec2(12.9898,78.233))) * 43758.5453), 0.0);
            shadow = ComputeShadow_Poisson(vec4(gbufferWorldPos, 1.0), randomRot);
            shadow = clamp(shadow, 0.0, 1.0);
        }
        Phong += Diffuse * shadow;
	}


	// Clamp light between 0-1 RGB values
	Phong = clamp(Phong, 0.0, 1.0);

	// Final color is texture color times phong light (alpha = 1)
	outColor = vec4(gbufferDiffuse * Phong, 1.0);
}
