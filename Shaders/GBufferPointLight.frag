

// Request GLSL 3.3
#version 330

// ���_�V�F�[�_�[����̓���
// �摜��UV���W
in vec2 fragTexCoord;

// �J���[�e�N�X�`���o��
layout(location = 0) out vec4 outColor;

// G-Buffer�e�N�X�`��
uniform sampler2D uGDiffuse;
uniform sampler2D uGNormal;
uniform sampler2D uGWorldPos;

// �|�C���g���C�g�̍\����
struct PointLight
{
	// Position of light
	vec3 mWorldPos;
	// Diffuse color
	vec3 mDiffuseColor;
	// Radius of the light
	float mInnerRadius;
	float mOuterRadius;
};

uniform PointLight uPointLight;
// ��ʃT�C�Y
uniform vec2 uScreenDimensions;

void main()
{
	// GBuffer��UV���W���v�Z
	vec2 gbufferCoord = gl_FragCoord.xy / uScreenDimensions;
	
	// GBuffer�e�N�X�`����������擾
	vec3 gbufferDiffuse = texture(uGDiffuse, gbufferCoord).xyz;
	vec3 gbufferNorm = texture(uGNormal, gbufferCoord).xyz;
	vec3 gbufferWorldPos = texture(uGWorldPos, gbufferCoord).xyz;
	
	// �@���̐��K��
	vec3 N = normalize(gbufferNorm);
	// ���C�g�����̌v�Z
	vec3 L = normalize(uPointLight.mWorldPos - gbufferWorldPos);

	// Phong�Ɩ����f��
	vec3 Phong = vec3(0.0, 0.0, 0.0);
	float NdotL = dot(N, L);
	if (NdotL > 0)
	{
		// ���ƃ��[���h�̈ʒu�Ƃ̋������擾
		float dist = distance(uPointLight.mWorldPos, gbufferWorldPos);
		// �X���[�X�X�e�b�v���g�p���āA�����a�ƊO���a�̊Ԃ�[0,1]�͈̔͂Œl���v�Z���܂�
		float intensity = smoothstep(uPointLight.mInnerRadius,
									 uPointLight.mOuterRadius, dist);
		// ���̋��x�𔽓]������i������1�A�O����0�j
		vec3 DiffuseColor = mix(uPointLight.mDiffuseColor,
								vec3(0.0, 0.0, 0.0), intensity);
		Phong = DiffuseColor * NdotL;
	}

	// �ŏI�I�ȃ��C�g����n�� (alpha = 1)
	outColor = vec4(gbufferDiffuse * Phong, 1.0);
}
