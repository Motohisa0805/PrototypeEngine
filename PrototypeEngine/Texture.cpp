#include "Texture.h"

Texture::Texture()
	:mTextureID(0)
	, mWidth(0)
	, mHeight(0)
{

}

Texture::~Texture()
{

}

bool Texture::Load(const string& fileName)
{
	int channels = 0;

	unsigned char* image = SOIL_load_image(fileName.c_str(),
		&mWidth, &mHeight, &channels, SOIL_LOAD_AUTO);

	if (image == nullptr)
	{
		SDL_Log("SOIL failed to load image %s: %s", fileName.c_str(), SOIL_last_result());
		return false;
	}

	int format = GL_RGB;
	if (channels == TextureLayout::GL_RGBA_SIZE)
	{
		format = GL_RGBA;
	}

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, format,
		GL_UNSIGNED_BYTE, image);

	SOIL_free_image_data(image);

	// Generate mipmaps for texture
	glGenerateMipmap(GL_TEXTURE_2D);
	// Enable linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Enable anisotropic filtering, if supported
	if (GLEW_EXT_texture_filter_anisotropic)
	{
		// Get the maximum anisotropy value
		GLfloat largest;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
		// Enable it
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest);
	}

	return true;
}

bool Texture::LoadFromAssimp(const aiTexture* embeddedTex)
{
	if (!embeddedTex || embeddedTex->mHeight != 0) 
	{
		SDL_Log("Invalid embedded texture.");
		return false;
	}

	int width, height, channels;
	unsigned char* image = SOIL_load_image_from_memory(
		reinterpret_cast<const unsigned char*>(embeddedTex->pcData),
		embeddedTex->mWidth, &width, &height, &channels, SOIL_LOAD_AUTO);

	if (!image) {
		SDL_Log("Failed to load embedded texture.");
		return false;
	}

	int format =			(channels == TextureLayout::GL_RGBA_SIZE) ? GL_RGBA :
							(channels == TextureLayout::GL_RGB_SIZE) ? GL_RGB :
							(channels == TextureLayout::GL_RED_SIZE) ? GL_RED : GL_RGB;

	int internalFormat =	(channels == TextureLayout::GL_RGBA8_SIZE) ? GL_RGBA8 :
							(channels == TextureLayout::GL_RGB8_SIZE) ? GL_RGB8 :
							(channels == TextureLayout::GL_R8_SIZE) ? GL_R8 : GL_RGB8;

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format,
		GL_UNSIGNED_BYTE, image);

	SOIL_free_image_data(image);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

bool Texture::LoadCubemapFromSingleImage(const std::string& fileName, int faceSize)
{
	int imgWidth, imgHeight, channels;
	unsigned char* image = SOIL_load_image(fileName.c_str(), &imgWidth, &imgHeight, &channels, SOIL_LOAD_AUTO);
	if (!image)
	{
		SDL_Log("SOIL failed to load cubemap image %s: %s", fileName.c_str(), SOIL_last_result());
		return false;
	}

	// �K�v�ȃt�H�[�}�b�g����
	int format = (channels == TextureLayout::GL_RGBA_SIZE) ? GL_RGBA : GL_RGB;

	// OpenGL�e�N�X�`���쐬
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);

	// �摜�̓N���X���C�A�E�g�i��4�A�c3�̃u���b�N�\���j��z��
	// Face�̏���: +X, -X, +Y, -Y, +Z, -Z
	struct FaceInfo {
		int x, y; // �u���b�N���W
	} faceOffsets[6] = {
		{2, 1}, // +X
		{0, 1}, // -X
		{1, 0}, // +Y
		{1, 2}, // -Y
		{1, 1}, // +Z
		{3, 1}  // -Z
	};

	// 1�ʂ̃s�N�Z���f�[�^��؂�o���o�b�t�@
	int bytesPerPixel = channels;
	std::vector<unsigned char> faceData(faceSize * faceSize * bytesPerPixel);

	for (int i = 0; i < 6; i++)
	{
		int startX = faceOffsets[i].x * faceSize;
		int startY = faceOffsets[i].y * faceSize;

		// �㉺�t�]�ɒ��ӁiSOIL��y�����]���Ă��Ȃ��̂�OpenGL�p�ɋt�ɂ���j
		for (int row = 0; row < faceSize; row++)
		{
			int srcY = startY + row;
			int dstY = faceSize - 1 - row;
			unsigned char* src = image + (srcY * imgWidth + startX) * bytesPerPixel;
			unsigned char* dst = faceData.data() + (dstY * faceSize) * bytesPerPixel;
			memcpy(dst, src, faceSize * bytesPerPixel);
		}

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, format, faceSize, faceSize, 0, format, GL_UNSIGNED_BYTE, faceData.data()
		);
	}

	SOIL_free_image_data(image);

	// Cubemap�p�����[�^�ݒ�
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return true;
}

bool Texture::LoadEquirectangularToCubemap(const std::string& fileName, int faceSize)
{
	// �\�[�X�摜��ǂݍ��ށiSOIL ���g�p�j
	int srcW = 0, srcH = 0, channels = 0;
	unsigned char* src = SOIL_load_image(fileName.c_str(), &srcW, &srcH, &channels, SOIL_LOAD_AUTO);
	if (!src)
	{
		SDL_Log("SOIL failed to load equirectangular image %s: %s", fileName.c_str(), SOIL_last_result());
		return false;
	}

	if (channels != 3 && channels != 4)
	{
		SDL_Log("Unsupported channel count %d in %s", channels, fileName.c_str());
		SOIL_free_image_data(src);
		return false;
	}

	// �K�v�ȓ���/�O���t�H�[�}�b�g
	GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
	GLenum internalFormat = (channels == 4) ? GL_RGBA8 : GL_RGB8;

	// Cubemap �e�N�X�`���쐬
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);

	// �e�ʂ̃s�N�Z���o�b�t�@
	std::vector<unsigned char> faceData(faceSize * faceSize * channels);

	// �e�t�F�C�X���Ƃ̕������v�Z���邽�߂� lambda
	auto getDirection = [](int face, float a, float b) -> Vector3
		{
			// a,b �� [-1,1] �ia: x�����Ab: y�����j
			Vector3 dir;
			switch (face)
			{
			case 0: // +X
				dir = Vector3(1.0f, -b, -a);
				break;
			case 1: // -X
				dir = Vector3(-1.0f, -b, a);
				break;
			case 2: // +Y
				dir = Vector3(a, 1.0f, b);
				break;
			case 3: // -Y
				dir = Vector3(a, -1.0f, -b);
				break;
			case 4: // +Z
				dir = Vector3(a, -b, 1.0f);
				break;
			case 5: // -Z
				dir = Vector3(-a, -b, -1.0f);
				break;
			default:
				dir = Vector3(0, 0, 0);
			}
			dir.Normalize();
			return dir;
		};

	const float PI = 3.14159265358979323846f;

	// face = 0..5 �̏��� POSITIVE_X .. NEGATIVE_Z �ɑΉ�
	for (int face = 0; face < 6; ++face)
	{
		// face �̊e�s�N�Z�����[�v
		for (int y = 0; y < faceSize; ++y)
		{
			// b �� -1..1 �Ɂi�� -> �� �� +�j
			float b = 2.0f * ((y + 0.5f) / static_cast<float>(faceSize)) - 1.0f;
			for (int x = 0; x < faceSize; ++x)
			{
				// a �� -1..1 �Ɂi�� -> �E �� +�j
				float a = 2.0f * ((x + 0.5f) / static_cast<float>(faceSize)) - 1.0f;

				Vector3 dir = getDirection(face, a, b);
				// �o�x�� = atan2(z, x), �ܓx�� = asin(y)
				float phi = atan2f(dir.z, dir.x);          // -PI..PI
				float theta = asinf(dir.y);                // -PI/2 .. PI/2

				// u,v �� 0..1 ��
				float u = (phi + PI) / (2.0f * PI);
				float v = (theta + (PI * 0.5f)) / PI;

				// �T���v�����O�i�o�C���j�A�j
				unsigned char pixel[4] = { 0,0,0,255 };
				Texture::SampleEquirect(src, srcW, srcH, channels, u, v, pixel);

				// faceData �Ɋi�[
				int idx = (y * faceSize + x) * channels;
				for (int c = 0; c < channels; ++c)
					faceData[idx + c] = pixel[c];
			}
		}

		// OpenGL �ɓ]��
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
			0, internalFormat, faceSize, faceSize, 0,
			format, GL_UNSIGNED_BYTE, faceData.data()
		);
	}

	// �\�[�X���
	SOIL_free_image_data(src);

	// �p�����[�^�ݒ�
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// �~�b�v����
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	return true;
}

void Texture::Unload()
{
	glDeleteTextures(1, &mTextureID);
}

void Texture::CreateFromSurface(SDL_Surface* surface)
{
	mWidth = surface->w;
	mHeight = surface->h;

	// Generate a GL texture
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_BGRA,
		GL_UNSIGNED_BYTE, surface->pixels);

	// Use linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::CreateForRendering(int width, int height, unsigned int format)
{
	mWidth = width;
	mHeight = height;
	// Create the texture id
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	// Set the image width/height with null initial data
	glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, GL_RGB,
		GL_FLOAT, nullptr);

	// For a texture we'll render to, just use nearest neighbor
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::SampleEquirect(const unsigned char* src, int srcW, int srcH, int channels, float u, float v, unsigned char* outPixel)
{
	// u, v �� [0,1]
// �摜�̌��_���㍶�̏ꍇ�Av �𔽓]����K�v�����邩������܂���B
// �����ł͈�ʓI�ȃC���[�W���C�u�����̏㌴�_�����肵�� v �� 1 - v �ɂ��Ă���_�ɒ��ӁB
	float fy = (1.0f - v) * (srcH - 1);
	float fx = u * (srcW - 1);

	int x0 = static_cast<int>(floorf(fx));
	int y0 = static_cast<int>(floorf(fy));
	int x1 = std::min(x0 + 1, srcW - 1);
	int y1 = std::min(y0 + 1, srcH - 1);

	float sx = fx - x0;
	float sy = fy - y0;

	for (int c = 0; c < channels; ++c)
	{
		float c00 = src[(y0 * srcW + x0) * channels + c];
		float c10 = src[(y0 * srcW + x1) * channels + c];
		float c01 = src[(y1 * srcW + x0) * channels + c];
		float c11 = src[(y1 * srcW + x1) * channels + c];

		float c0 = c00 * (1.0f - sx) + c10 * sx;
		float c1 = c01 * (1.0f - sx) + c11 * sx;
		float cf = c0 * (1.0f - sy) + c1 * sy;

		outPixel[c] = static_cast<unsigned char>(std::round(cf));
	}
}

void Texture::SetActive(int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}

void Texture::SetNoActive(int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, 0);
}