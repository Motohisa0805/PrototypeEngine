

// Request GLSL 3.3
#version 330
//スケルタルメッシュの頂点シェーダー
// ワールド変換とビュー射影を行います
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;
// 行列優先で96個のボーンマトリクスを受け取る
uniform mat4 uMatrixPalette[96];

// 0 座標, 1 法線, 2 スキニング用ボーンインデックス, 3 スキニング用ボーンウェイト, 4 テクスチャ座標
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in uvec4 inSkinBones;
layout(location = 3) in vec4 inSkinWeights;
layout(location = 4) in vec2 inTexCoord;

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
	
	// スキンニング処理
	vec4 skinnedPos = (pos * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;

	// 座標をワールド変換
	skinnedPos = skinnedPos * uWorldTransform;
	// ワールド座標をフラグメントシェーダーに渡す
	fragWorldPos = skinnedPos.xyz;
	// 座標をクリップ空間に変換
	gl_Position = skinnedPos * uViewProj;

	// 頂点法線のスキンニング処理
	vec4 skinnedNormal = vec4(inNormal, 0.0f);
	skinnedNormal = (skinnedNormal * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x
		+ (skinnedNormal * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y
		+ (skinnedNormal * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z
		+ (skinnedNormal * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;
	// ワールド変換に伴う法線の変換
	mat3 normalMatrix = transpose(inverse(mat3(uWorldTransform)));
	fragNormal = normalize(normalMatrix * inNormal);

	// フラグメントシェーダーにテクスチャ座標を渡す
	fragTexCoord = inTexCoord;
}

