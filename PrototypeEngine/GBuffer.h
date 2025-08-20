#pragma once
#include "Texture.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

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
	vector<class Texture*>		mTextures;
	// Frame buffer object ID
	unsigned int				mBufferID;
public:

								GBuffer();
								~GBuffer();

	// Create/destroy the G-buffer
	bool						Create(int width, int height);
	void						Destroy();

	// Get the texture for a specific type of data
	class Texture*				GetTexture(Type type);
	// Get the framebuffer object ID
	unsigned int				GetBufferID() const { return mBufferID; }
	// Setup all the G-buffer textures for sampling
	void						SetTexturesActive();
};