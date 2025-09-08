

// Request GLSL 3.3
#version 330

// ワールド座標変換とビュー射影のためのユニフォーム
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

//0 座標, 1 法線, 2 テクスチャ座標
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

//位置以外の任意の頂点出力
out vec2 fragTexCoord;

out vec3 fragNormal;
out vec3 fragWorldPos;

void main()
{
	// 位置を vec4 に拡張
	vec4 worldPos = vec4(inPosition, 1.0) * uWorldTransform;
	gl_Position = worldPos * uViewProj;

	// シェーダーにテクスチャ座標と法線、ワールド座標を渡す
	fragTexCoord = inTexCoord;
	fragNormal = mat3(transpose(inverse(uWorldTransform))) * inNormal;

	fragWorldPos = worldPos.xyz;
}
