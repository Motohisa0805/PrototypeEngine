
// �摜�`��Ɏg��vert�V�F�[�_�[
// Request GLSL 3.3
#version 330

// ���[���h���W�ϊ��ƃr���[�ˉe�̂��߂̃��j�t�H�[��
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;
uniform vec4 uTexUV; // .xy = offset, .zw = scale

// 0 ���W, 1 �@��, 2 �e�N�X�`�����W
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// ���_�V�F�[�_�[����t���O�����g�V�F�[�_�[�ɓn���ϐ�
out vec2 fragTexCoord;

void main()
{
	// Vect4�Ɋg��
	vec4 pos = vec4(inPosition, 1.0);
	// ���[���h�ϊ����Ă���A�N���b�v��Ԃɕϊ�
	gl_Position = pos * uWorldTransform * uViewProj;

	// �t���O�����g�V�F�[�_�[�Ƀe�N�X�`�����W��n��
	fragTexCoord = inTexCoord * uTexUV.zw + uTexUV.xy;
}
