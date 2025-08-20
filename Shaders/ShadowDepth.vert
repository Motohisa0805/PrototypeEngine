//シャドウマップの頂点シェーダー
#version 330 core

layout(location = 0) in vec3 inPosition;

uniform mat4 uWorldTransform;
uniform mat4 uLightViewProj;

void main()
{
    gl_Position = vec4(inPosition, 1.0) * uWorldTransform * uLightViewProj;
}