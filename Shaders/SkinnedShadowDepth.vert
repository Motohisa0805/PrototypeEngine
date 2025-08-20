//スケルタルメッシュのシャドウマップ生成用頂点シェーダー
#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 2) in uvec4 inSkinBones;
layout(location = 3) in vec4 inSkinWeights;

uniform mat4 uMatrixPalette[96];
uniform mat4 uWorldTransform;
uniform mat4 uLightViewProj;

void main()
{
    mat4 skinMat =
    uMatrixPalette[inSkinBones.x] * inSkinWeights.x +
    uMatrixPalette[inSkinBones.y] * inSkinWeights.y +
    uMatrixPalette[inSkinBones.z] * inSkinWeights.z +
    uMatrixPalette[inSkinBones.w] * inSkinWeights.w;

    gl_Position = vec4(inPosition, 1.0) * skinMat * uWorldTransform * uLightViewProj;
}