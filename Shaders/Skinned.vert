

// Request GLSL 3.3
#version 330
//�X�P���^�����b�V���̒��_�V�F�[�_�[
// ���[���h�ϊ��ƃr���[�ˉe���s���܂�
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;
// �s��D���96�̃{�[���}�g���N�X���󂯎��
uniform mat4 uMatrixPalette[96];

// 0 ���W, 1 �@��, 2 �X�L�j���O�p�{�[���C���f�b�N�X, 3 �X�L�j���O�p�{�[���E�F�C�g, 4 �e�N�X�`�����W
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in uvec4 inSkinBones;
layout(location = 3) in vec4 inSkinWeights;
layout(location = 4) in vec2 inTexCoord;

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
	
	// �X�L���j���O����
	vec4 skinnedPos = (pos * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;

	// ���W�����[���h�ϊ�
	skinnedPos = skinnedPos * uWorldTransform;
	// ���[���h���W���t���O�����g�V�F�[�_�[�ɓn��
	fragWorldPos = skinnedPos.xyz;
	// ���W���N���b�v��Ԃɕϊ�
	gl_Position = skinnedPos * uViewProj;

	// ���_�@���̃X�L���j���O����
	vec4 skinnedNormal = vec4(inNormal, 0.0f);
	skinnedNormal = (skinnedNormal * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x
		+ (skinnedNormal * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y
		+ (skinnedNormal * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z
		+ (skinnedNormal * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;
	// ���[���h�ϊ��ɔ����@���̕ϊ�
	mat3 normalMatrix = transpose(inverse(mat3(uWorldTransform)));
	fragNormal = normalize(normalMatrix * inNormal);

	// �t���O�����g�V�F�[�_�[�Ƀe�N�X�`�����W��n��
	fragTexCoord = inTexCoord;
}

