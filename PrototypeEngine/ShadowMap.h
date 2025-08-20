#pragma once
#include "Texture.h"
#include "WindowRenderProperty.h"

//�V���h�E�}�b�s���O�̂��߂̃N���X
// �t���[���o�b�t�@�I�u�W�F�N�g(FBO)�Ɛ[�x�e�N�X�`�����Ǘ�
class ShadowMap
{
private:
	GLuint mFBO;
	GLuint mDepthTexture;
	int mWidth;
	int mHeight;
	Matrix4 mLightViewProj;
public:
	ShadowMap();
	~ShadowMap();

	// FBO, DepthTexture����
	bool Initialize(int width, int height);        
	// FBO�o�C���h�{�N���A
	void BeginRender();                            
	// FBO����
	void EndRender();                              
	GLuint GetDepthTexture() const { return mDepthTexture; }
	Matrix4 GetLightViewProj() const { return mLightViewProj; }

	void UpdateLightMatrix(const Vector3& lightDir, const Vector3& target = Vector3::Zero);
};

