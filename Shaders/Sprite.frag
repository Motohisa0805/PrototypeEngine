
// �摜�`��Ɏg��frag�V�F�[�_�[
// Request GLSL 3.3
#version 330

// ���_�V�F�[�_�[����̓���
in vec2 fragTexCoord;

// �J���[�e�N�X�`���o��
out vec4 outColor;

// �摜�e�N�X�`��
uniform sampler2D uTexture;

void main()
{
	// �摜�e�N�X�`���̐F�����̂܂܏o��
    outColor = texture(uTexture, fragTexCoord);
}
