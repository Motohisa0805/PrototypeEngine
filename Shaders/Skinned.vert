

// Request GLSL 3.3
#version 330
//スケルタルメッシュの頂点シェーダー
// Uniforms for world transform and view-proj
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;
// Uniform for matrix palette
uniform mat4 uMatrixPalette[96];

// Attribute 0 is position, 1 is normal,
// 2 is bone indices, 3 is weights,
// 4 is tex coords.
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
	// Convert position to homogeneous coordinates
	vec4 pos = vec4(inPosition, 1.0);
	
	// Skin the position
	vec4 skinnedPos = (pos * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;

	// Transform position to world space
	skinnedPos = skinnedPos * uWorldTransform;
	// Save world position
	fragWorldPos = skinnedPos.xyz;
	// Transform to clip space
	gl_Position = skinnedPos * uViewProj;

	// Skin the vertex normal
	vec4 skinnedNormal = vec4(inNormal, 0.0f);
	skinnedNormal = (skinnedNormal * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x
		+ (skinnedNormal * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y
		+ (skinnedNormal * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z
		+ (skinnedNormal * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;
	// Transform normal into world space (w = 0)
	mat3 normalMatrix = transpose(inverse(mat3(uWorldTransform)));
	fragNormal = normalize(normalMatrix * inNormal);
	//fragNormal = (skinnedNormal * uWorldTransform).xyz;

	// Pass along the texture coordinate to frag shader
	fragTexCoord = inTexCoord;
}

