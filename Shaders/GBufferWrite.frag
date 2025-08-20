

// Request GLSL 3.3
#version 330
//���b�V���p�̃t���O�V�F�[�_�[
// Inputs from vertex shader
// Tex coord
in vec2 fragTexCoord;
// Normal (in world space)
in vec3 fragNormal;
// Position (in world space)
in vec3 fragWorldPos;

// This corresponds to the outputs to the G-buffer
layout(location = 0) out vec4 outDiffuse;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outWorldPos;

// Uniforms
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
	
	// Normal/world pos are passed directly along
	outNormal = fragNormal;
	outWorldPos = fragWorldPos;

	// // �e�N�X�`���J���[�ialbedo�j
    //vec4 texColor = texture(uTexture, fragTexCoord);
    //
    //// �@���𐳋K��
    //vec3 normal = normalize(fragNormal);
    //
    //// ���̕����i������������ɔ��]�j
    //vec3 lightDir = normalize(-uLightDir);
    //
    //// ���������i�J���� �� �t���O�����g�j
    //vec3 viewDir = normalize(uViewPos - fragWorldPos);
    //
    //// ���˃x�N�g���i�������˂�������j
    //vec3 reflectDir = reflect(-lightDir, normal);
    //
    //// ----- ���� -----
    //vec3 ambient = ambientColor * texColor.rgb;
    //
    //// ----- �g�U���iLambert�j-----
    //float diff = max(dot(normal, lightDir), 0.0);
    //vec3 diffuse = diffuseColor * diff * texColor.rgb;
    //
    //// ----- ���ʔ��ˌ��iPhong�j-----
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    //vec3 specular = spec * specularColor;
    //
    //// ----- ���� -----
    //vec3 finalColor = ambient + diffuse + specular;
    //
    //outDiffuse = vec4(finalColor, uColor.a * texColor.a);
    //outNormal = normal;
    //outWorldPos = fragWorldPos;

}
