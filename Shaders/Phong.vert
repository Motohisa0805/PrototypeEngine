

// Request GLSL 3.3
#version 330
//���b�V���p�̊�{�I�Ȓ��_�V�F�[�_�[
// ���W�Ɩ@���A�e�N�X�`�����W���󂯎��A���[���h�ϊ��ƃr���[�ˉe���s���܂�
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// 0 ���W, 1 �@��, 2 �e�N�X�`�����W
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// UV���W
out vec2 fragTexCoord;
// �@��
out vec3 fragNormal;
// �t���O�����g�ʒu�i���[���h���W�j
out vec3 fragWorldPos;

void main()
{
	// Vect4�Ɋg��
	vec4 pos = vec4(inPosition, 1.0);
	// ���W�����[���h�ϊ�
	pos = pos * uWorldTransform;
	// ���[���h���W���t���O�����g�V�F�[�_�[�ɓn��
	fragWorldPos = pos.xyz;
	// ���W���N���b�v��Ԃɕϊ�
	gl_Position = pos * uViewProj;

	// ���W�ϊ��ɔ����@���̕ϊ�
	mat3 normalMatrix = transpose(inverse(mat3(uWorldTransform)));
	fragNormal = normalize(normalMatrix * inNormal);

	// �V�F�[�_�[�Ƀe�N�X�`�����W��n��
	fragTexCoord = inTexCoord;
}
