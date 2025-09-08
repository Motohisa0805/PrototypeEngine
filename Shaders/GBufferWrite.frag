

// Request GLSL 3.3
#version 330
//メッシュ用のフラグシェーダー
// 頂点シェーダーからの入力
// 画像のUV座標
in vec2 fragTexCoord;
// フラグメントの法線（ワールド空間）
in vec3 fragNormal;
// 座標（ワールド空間）
in vec3 fragWorldPos;

// G-Buffer出力
layout(location = 0) out vec4 outDiffuse;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outWorldPos;

// ユニフォーム
uniform sampler2D uTexture;

uniform vec4 uColor; // マテリアルカラー（アルファ含む）

// ライト方向（ワールド空間、単位ベクトル）
// 例: vec3(1, -1, 0) など。必ず normalize() されたものを送る
uniform vec3 uLightDir;

// カメラ位置（視線ベクトルの計算に必要）
uniform vec3 uViewPos;

// ライトの色情報
uniform vec3 ambientColor;   // 環境光
uniform vec3 diffuseColor;   // 拡散光
uniform vec3 specularColor;  // 鏡面反射光
uniform float shininess;     // 鏡面反射の鋭さ


void main()
{

	vec4 texColor = texture(uTexture, fragTexCoord);
	
	vec3 ambient = ambientColor * texColor.rgb;
	vec3 diffuse = diffuseColor * texColor.rgb;
	vec3 specular = specularColor * shininess;
	
	vec3 finalColor = ambient + diffuse + specular;
	
	// 不透明度 = マテリアルカラー × テクスチャアルファ
	outDiffuse = vec4(finalColor, uColor.a * texColor.a);
	
	// 法線とワールド座標をそのまま出力
	outNormal = fragNormal;
	outWorldPos = fragWorldPos;
}
