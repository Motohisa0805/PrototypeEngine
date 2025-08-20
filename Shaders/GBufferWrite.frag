

// Request GLSL 3.3
#version 330
//メッシュ用のフラグシェーダー
// Inputs from vertex shader
// Tex coord
in vec2 fragTexCoord;
// Normal (in world space)
in vec3 fragNormal;
// Position (in world space)
in vec3 fragWorldPos;

// This corresponds to the outputs to the G-buffer
layout(location = 0) out vec4 outDiffuse;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outWorldPos;

// Uniforms
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
	
	// Normal/world pos are passed directly along
	outNormal = fragNormal;
	outWorldPos = fragWorldPos;

	// // テクスチャカラー（albedo）
    //vec4 texColor = texture(uTexture, fragTexCoord);
    //
    //// 法線を正規化
    //vec3 normal = normalize(fragNormal);
    //
    //// 光の方向（光が来る方向に反転）
    //vec3 lightDir = normalize(-uLightDir);
    //
    //// 視線方向（カメラ → フラグメント）
    //vec3 viewDir = normalize(uViewPos - fragWorldPos);
    //
    //// 反射ベクトル（光が反射する方向）
    //vec3 reflectDir = reflect(-lightDir, normal);
    //
    //// ----- 環境光 -----
    //vec3 ambient = ambientColor * texColor.rgb;
    //
    //// ----- 拡散光（Lambert）-----
    //float diff = max(dot(normal, lightDir), 0.0);
    //vec3 diffuse = diffuseColor * diff * texColor.rgb;
    //
    //// ----- 鏡面反射光（Phong）-----
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    //vec3 specular = spec * specularColor;
    //
    //// ----- 合成 -----
    //vec3 finalColor = ambient + diffuse + specular;
    //
    //outDiffuse = vec4(finalColor, uColor.a * texColor.a);
    //outNormal = normal;
    //outWorldPos = fragWorldPos;

}
