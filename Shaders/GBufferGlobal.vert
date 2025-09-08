

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

out vec3 fragNormal;
out vec3 fragWorldPos;

void main()
{
	// �ʒu�� vec4 �Ɋg��
	vec4 worldPos = vec4(inPosition, 1.0) * uWorldTransform;
	gl_Position = worldPos * uViewProj;

	// �V�F�[�_�[�Ƀe�N�X�`�����W�Ɩ@���A���[���h���W��n��
	fragTexCoord = inTexCoord;
	fragNormal = mat3(transpose(inverse(uWorldTransform))) * inNormal;

	fragWorldPos = worldPos.xyz;
}
