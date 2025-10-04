#pragma once
#include "Texture.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class Texture;

//書籍元のファイル
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
	// Gバッファに関連するテクスチャ
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

	//ImGui に渡すための関数を追加
	GLuint						GetImGuiColorAttachment(int index = 0) const;
};