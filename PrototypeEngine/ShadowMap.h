#pragma once
#include "Texture.h"
#include "WindowRenderProperty.h"

//シャドウマッピングのためのクラス
// フレームバッファオブジェクト(FBO)と深度テクスチャを管理
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

	// FBO, DepthTexture生成
	bool Initialize(int width, int height);        
	// FBOバインド＋クリア
	void BeginRender();                            
	// FBO解除
	void EndRender();                              
	GLuint GetDepthTexture() const { return mDepthTexture; }
	Matrix4 GetLightViewProj() const { return mLightViewProj; }

	void UpdateLightMatrix(const Vector3& lightDir, const Vector3& target = Vector3::Zero);
};

