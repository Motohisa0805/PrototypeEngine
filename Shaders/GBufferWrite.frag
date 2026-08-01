

// Request GLSL 3.3
//PBRの考え方のもと、G-Bufferに必要な情報を出力するフラグメントシェーダーに改良予定
#version 330
//メッシュ用のフラグシェーダー
// 頂点シェーダーからの入力
// 画像のUV座標
in vec2 fragTexCoord;
// フラグメントの法線（ワールド空間）
in vec3 fragNormal;
// 座標（ワールド空間）
in vec3 fragWorldPos;

// G-Buffer出力(RGBA16FやRGBA8などのフォーマットはC++側で設定)
layout(location = 0) out vec4 outDiffuse;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outWorldPos;
layout(location = 3) out vec4 outPBR; // メタリックとラフネスを格納するターゲット
layout(location = 4) out vec4 outEmissive; // エミッションを格納するターゲット

// ユニフォーム
uniform sampler2D	uTexture;

uniform vec4		uColor; // マテリアルカラー（アルファ含む）

uniform float		uMetallic;
uniform float		uRoughness;
uniform vec3		uEmissive;


void main()
{

	vec4 albedo = texture(uTexture, fragTexCoord) * uColor;
	
	if(albedo.a < 0.01)
	{
		discard;
	}

	vec3 realDiffuse = mix(albedo.rgb,vec3(0.0),uMetallic);
	vec3 realSpecular = mix(vec3(0.0),albedo.rgb,uMetallic);

	// 不透明度 = マテリアルカラー × テクスチャアルファ
	outDiffuse = vec4(realDiffuse,albedo.a);
	// 法線とワールド座標をそのまま出力
	outNormal = normalize(fragNormal);
	outWorldPos = fragWorldPos;
	outPBR = vec4(realSpecular,uRoughness);
	outEmissive = vec4(uEmissive,1.0);
}
