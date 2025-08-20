

// Request GLSL 3.3
#version 330
//メッシュ用の基本的な頂点シェーダー
// Uniforms for world transform and view-proj
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// Attribute 0 is position, 1 is normal, 2 is tex coords.
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
	// Convert position to homogeneous coordinates
	vec4 pos = vec4(inPosition, 1.0);
	// Transform position to world space
	pos = pos * uWorldTransform;
	// Save world position
	fragWorldPos = pos.xyz;
	// Transform to clip space
	gl_Position = pos * uViewProj;

	// Transform normal into world space (w = 0)
	mat3 normalMatrix = transpose(inverse(mat3(uWorldTransform)));
	fragNormal = normalize(normalMatrix * inNormal);
	//fragNormal = (vec4(inNormal, 0.0f) * uWorldTransform).xyz;

	// Pass along the texture coordinate to frag shader
	fragTexCoord = inTexCoord;
}
