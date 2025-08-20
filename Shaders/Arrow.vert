#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;

uniform mat4 uModel;
uniform mat4 uViewProj;

out vec3 vColor;

void main()
{
    gl_Position = vec4(aPosition, 1.0) * uModel * uViewProj;
    vColor = aColor;
}