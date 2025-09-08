

// Request GLSL 3.3
#version 330

// ���[���h���W�ϊ��ƃr���[�ˉe�̂��߂̃��j�t�H�[��
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

//0 ���W, 1 �@��, 2 �e�N�X�`�����W
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;


//�ʒu�ȊO�̔C�ӂ̒��_�o��
out vec2 fragTexCoord;

void main()
{
	// ���W�� vec4 �Ɋg��
	vec4 pos = vec4(inPosition, 1.0);
	// ���[���h�X�y�[�X�ɕϊ����Ă���A�N���b�v�X�y�[�X�ɂ��܂�
	gl_Position = pos * uWorldTransform * uViewProj;

	// �e�N�X�`�����W���t���O�����g�V�F�[�_�[�ɓn��
	fragTexCoord = inTexCoord;
}
