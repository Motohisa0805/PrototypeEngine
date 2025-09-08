

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

// ポイントライトの構造体
struct PointLight
{
	// Position of light
	vec3 mWorldPos;
	// Diffuse color
	vec3 mDiffuseColor;
	// Radius of the light
	float mInnerRadius;
	float mOuterRadius;
};

uniform PointLight uPointLight;
// 画面サイズ
uniform vec2 uScreenDimensions;

void main()
{
	// GBufferのUV座標を計算
	vec2 gbufferCoord = gl_FragCoord.xy / uScreenDimensions;
	
	// GBufferテクスチャから情報を取得
	vec3 gbufferDiffuse = texture(uGDiffuse, gbufferCoord).xyz;
	vec3 gbufferNorm = texture(uGNormal, gbufferCoord).xyz;
	vec3 gbufferWorldPos = texture(uGWorldPos, gbufferCoord).xyz;
	
	// 法線の正規化
	vec3 N = normalize(gbufferNorm);
	// ライト方向の計算
	vec3 L = normalize(uPointLight.mWorldPos - gbufferWorldPos);

	// Phong照明モデル
	vec3 Phong = vec3(0.0, 0.0, 0.0);
	float NdotL = dot(N, L);
	if (NdotL > 0)
	{
		// 光とワールドの位置との距離を取得
		float dist = distance(uPointLight.mWorldPos, gbufferWorldPos);
		// スムースステップを使用して、内半径と外半径の間の[0,1]の範囲で値を計算します
		float intensity = smoothstep(uPointLight.mInnerRadius,
									 uPointLight.mOuterRadius, dist);
		// 光の強度を反転させる（内側が1、外側が0）
		vec3 DiffuseColor = mix(uPointLight.mDiffuseColor,
								vec3(0.0, 0.0, 0.0), intensity);
		Phong = DiffuseColor * NdotL;
	}

	// 最終的なライト情報を渡す (alpha = 1)
	outColor = vec4(gbufferDiffuse * Phong, 1.0);
}
