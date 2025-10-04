#include "GBuffer.h"

GBuffer::GBuffer()
	:mBufferID(0)
	, mDepthBuffer(0)
	, mWidth(0)
	, mHeight(0)
{

}

GBuffer::~GBuffer()
{

}

bool GBuffer::Create(int width, int height)
{
	// Create the framebuffer object
	glGenFramebuffers(1, &mBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);

	// Add a depth buffer to this target
	glGenRenderbuffers(1, &mDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
		width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, mDepthBuffer);

	// Create textures for each output in the G-buffer
	for (int i = 0; i < NUM_GBUFFER_TEXTURES; i++)
	{
		Texture* tex = new Texture();
		// We want three 32-bit float components for each texture
		tex->CreateForRendering(width, height, GL_RGB32F);
		mTextures.emplace_back(tex);
		// Attach this texture to a color output
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
			tex->GetTextureID(), 0);
	}

	// Create a vector of the color attachments
	vector<GLenum> attachments;
	for (int i = 0; i < NUM_GBUFFER_TEXTURES; i++)
	{
		attachments.emplace_back(GL_COLOR_ATTACHMENT0 + i);
	}

	// Set the list of buffers to draw to
	glDrawBuffers(static_cast<GLsizei>(attachments.size()),
		attachments.data());

	// Make sure everything worked
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Destroy();
		return false;
	}

	return true;
}

bool GBuffer::Resize(int width, int height)
{
	mWidth = width;
	mHeight = height;

	// 既存のFBOをバインド
	glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);

	// デプスバッファを再確保
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	// 既存のG-Bufferテクスチャをサイズ変更
	for (int i = 0; i < NUM_GBUFFER_TEXTURES; i++)
	{
		glBindTexture(GL_TEXTURE_2D, mTextures[i]->GetTextureID());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0,
			GL_RGB, GL_FLOAT, nullptr);
	}

	// FBOがまだ有効か確認（必須）
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("GBuffer resize failed!\n");
		Destroy();
		return false;
	}

	// FBOをアンバインド
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void GBuffer::Destroy()
{
	glDeleteFramebuffers(1, &mBufferID);
	for (Texture* t : mTextures)
	{
		t->Unload();
		delete t;
	}
}

Texture* GBuffer::GetTexture(Type type)
{
	if (mTextures.size() > 0)
	{
		return mTextures[type];
	}
	else
	{
		return nullptr;
	}
}

void GBuffer::SetTexturesActive()
{
	for (int i = 0; i < NUM_GBUFFER_TEXTURES; i++)
	{
		mTextures[i]->SetActive(i);
	}
}

bool GBuffer::NeedsResize(Vector2 size)
{
	bool b = (size.x != mWidth || size.y != mHeight);
	return b;
}

GLuint GBuffer::GetImGuiColorAttachment(int index) const
{
	if (index < 0 || index >= (int)mTextures.size())
	{
		return 0;
	}
	return mTextures[index]->GetTextureID();
}
