
// 画像描画に使うfragシェーダー
// Request GLSL 3.3
#version 330

// 頂点シェーダーからの入力
in vec2 fragTexCoord;

// カラーテクスチャ出力
out vec4 outColor;

// 画像テクスチャ
uniform sampler2D uTexture;

void main()
{
	// 画像テクスチャの色をそのまま出力
    outColor = texture(uTexture, fragTexCoord);
}
