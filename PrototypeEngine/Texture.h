#pragma once
#include "Assimp.h"
#include "SOIL.h"
#include <GL/glew.h>
#include "SDL3.h"
#include "FilePath.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

// OpenGLのテクスチャのフォーマットを定義する定数
namespace TextureLayout
{
	constexpr int GL_RGBA_SIZE = 4;

	constexpr int GL_RGB_SIZE = 3;

	constexpr int GL_RED_SIZE = 1;

	constexpr int GL_RGBA8_SIZE = 4;

	constexpr int GL_RGB8_SIZE = 3;

	constexpr int GL_R8_SIZE = 1;
}

//書籍元を改造したファイル
//画像の読み込み、参照を行うことをカプセル化したクラス
// OpenGLのテクスチャを管理するクラス
class Texture
{
private:
	unsigned int	mTextureID;

	int				mWidth;
	
	int				mHeight;
public:
					Texture();
					~Texture();

	bool			Load(const string& fileName);
	bool			LoadFromAssimp(const aiTexture* embeddedTex);
	bool			LoadCubemapFromSingleImage(const std::string& fileName, int faceSize);
	bool			LoadEquirectangularToCubemap(const std::string& fileName, int faceSize);
	void			Unload();
	void			CreateFromSurface(struct SDL_Surface* surface);
	void			CreateForRendering(int width, int height, unsigned int format);

	static void SampleEquirect(const unsigned char* src, int srcW, int srcH, int channels,float u, float v, unsigned char* outPixel);

	void			SetActive(int index = 0);
	void			SetNoActive(int index = 0);

	int				GetWidth() const { return mWidth; }
	int				GetHeight() const { return mHeight; }
	unsigned int	GetTextureID() const { return mTextureID; }
};