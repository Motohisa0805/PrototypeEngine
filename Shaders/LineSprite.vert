

#version 330 core
layout(location = 0) in vec3 inPosition;

uniform mat4 uViewProj;
uniform mat4 uWorldTransform;

void main()
{
    gl_Position = uViewProj * uWorldTransform * vec4(inPosition, 1.0);
}
