
// 画像描画に使うvertシェーダー
// Request GLSL 3.3
#version 330

// ワールド座標変換とビュー射影のためのユニフォーム
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;
uniform vec4 uTexUV; // .xy = offset, .zw = scale

// 0 座標, 1 法線, 2 テクスチャ座標
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// 頂点シェーダーからフラグメントシェーダーに渡す変数
out vec2 fragTexCoord;

void main()
{
	// Vect4に拡張
	vec4 pos = vec4(inPosition, 1.0);
	// ワールド変換してから、クリップ空間に変換
	gl_Position = pos * uWorldTransform * uViewProj;

	// フラグメントシェーダーにテクスチャ座標を渡す
	fragTexCoord = inTexCoord * uTexUV.zw + uTexUV.xy;
}
