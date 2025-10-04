#pragma once
#include "Texture.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//�O���錾
class Texture;

//���Ќ��̃t�@�C��
class GBuffer
{
public:
	// Different types of data stored in the G-buffer
	enum Type
	{
		EDiffuse = 0,
		ENormal,
		EWorldPos,
		NUM_GBUFFER_TEXTURES
	};
private:
	// G�o�b�t�@�Ɋ֘A����e�N�X�`��
	vector<Texture*>			mTextures;
	// Frame buffer object ID
	unsigned int				mBufferID;

	GLuint						mDepthBuffer;
	int 						mWidth;
	int 						mHeight;
public:

								GBuffer();
								~GBuffer();

	// Create/destroy the G-buffer
	bool						Create(int width, int height);
	bool						Resize(int width, int height);
	void						Destroy();

	// Get the texture for a specific type of data
	Texture*					GetTexture(Type type);
	// Get the framebuffer object ID
	unsigned int				GetBufferID() const { return mBufferID; }
	// Setup all the G-buffer textures for sampling
	void						SetTexturesActive();

	bool						NeedsResize(Vector2 size);

	//ImGui �ɓn�����߂̊֐���ǉ�
	GLuint						GetImGuiColorAttachment(int index = 0) const;
};