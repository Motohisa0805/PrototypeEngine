

// Request GLSL 3.3
#version 330
//���b�V���p�̃t���O�V�F�[�_�[
// ���_�V�F�[�_�[����̓���
// �摜��UV���W
in vec2 fragTexCoord;
// �t���O�����g�̖@���i���[���h��ԁj
in vec3 fragNormal;
// ���W�i���[���h��ԁj
in vec3 fragWorldPos;

// G-Buffer�o��
layout(location = 0) out vec4 outDiffuse;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outWorldPos;

// ���j�t�H�[��
uniform sampler2D uTexture;

uniform vec4 uColor; // �}�e���A���J���[�i�A���t�@�܂ށj

// ���C�g�����i���[���h��ԁA�P�ʃx�N�g���j
// ��: vec3(1, -1, 0) �ȂǁB�K�� normalize() ���ꂽ���̂𑗂�
uniform vec3 uLightDir;

// �J�����ʒu�i�����x�N�g���̌v�Z�ɕK�v�j
uniform vec3 uViewPos;

// ���C�g�̐F���
uniform vec3 ambientColor;   // ����
uniform vec3 diffuseColor;   // �g�U��
uniform vec3 specularColor;  // ���ʔ��ˌ�
uniform float shininess;     // ���ʔ��˂̉s��


void main()
{

	vec4 texColor = texture(uTexture, fragTexCoord);
	
	vec3 ambient = ambientColor * texColor.rgb;
	vec3 diffuse = diffuseColor * texColor.rgb;
	vec3 specular = specularColor * shininess;
	
	vec3 finalColor = ambient + diffuse + specular;
	
	// �s�����x = �}�e���A���J���[ �~ �e�N�X�`���A���t�@
	outDiffuse = vec4(finalColor, uColor.a * texColor.a);
	
	// �@���ƃ��[���h���W�����̂܂܏o��
	outNormal = fragNormal;
	outWorldPos = fragWorldPos;
}
