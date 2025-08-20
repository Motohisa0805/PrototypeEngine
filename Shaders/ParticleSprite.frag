
// �摜�`��Ɏg��frag�V�F�[�_�[
// Request GLSL 3.3
#version 330

in vec2 fragTexCoord;
out vec4 outColor;

uniform sampler2D uTexture;
uniform vec4 uColor; // �� �ǉ��F�O������F�{�����󂯎��

void main()
{
    vec4 texColor = texture(uTexture, fragTexCoord);

    // �e�N�X�`���F�� uColor ����Z�i�����܂߂āj
    outColor = texColor * uColor;
}