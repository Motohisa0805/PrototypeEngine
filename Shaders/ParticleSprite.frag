
// 画像描画に使うfragシェーダー
// Request GLSL 3.3
#version 330

in vec2 fragTexCoord;
out vec4 outColor;

uniform sampler2D uTexture;
uniform vec4 uColor; // ← 追加：外部から色＋αを受け取る

void main()
{
    vec4 texColor = texture(uTexture, fragTexCoord);

    // テクスチャ色に uColor を乗算（αも含めて）
    outColor = texColor * uColor;
}