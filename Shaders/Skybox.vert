//�X�J�C�{�b�N�X�̒��_�V�F�[�_�[
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 uView;
uniform mat4 uProj;

void main()
{
    TexCoords = aPos;
    vec4 pos = vec4(aPos, 1.0) * uView * uProj;
    gl_Position = pos.xyww; // �[�x���ő�ɌŒ�
}