

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

void main()
{
	// 座標を vec4 に拡張
	vec4 pos = vec4(inPosition, 1.0);
	// ワールドスペースに変換してから、クリップスペースにします
	gl_Position = pos * uWorldTransform * uViewProj;

	// テクスチャ座標をフラグメントシェーダーに渡す
	fragTexCoord = inTexCoord;
}
