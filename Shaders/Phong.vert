

// Request GLSL 3.3
#version 330
//メッシュ用の基本的な頂点シェーダー
// 座標と法線、テクスチャ座標を受け取り、ワールド変換とビュー射影を行います
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// 0 座標, 1 法線, 2 テクスチャ座標
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// UV座標
out vec2 fragTexCoord;
// 法線
out vec3 fragNormal;
// フラグメント位置（ワールド座標）
out vec3 fragWorldPos;

void main()
{
	// Vect4に拡張
	vec4 pos = vec4(inPosition, 1.0);
	// 座標をワールド変換
	pos = pos * uWorldTransform;
	// ワールド座標をフラグメントシェーダーに渡す
	fragWorldPos = pos.xyz;
	// 座標をクリップ空間に変換
	gl_Position = pos * uViewProj;

	// 座標変換に伴う法線の変換
	mat3 normalMatrix = transpose(inverse(mat3(uWorldTransform)));
	fragNormal = normalize(normalMatrix * inNormal);

	// シェーダーにテクスチャ座標を渡す
	fragTexCoord = inTexCoord;
}
