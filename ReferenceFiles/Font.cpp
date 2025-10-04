#include "Font.h"

Font::Font()
	:mGame(SceneManager::GetNowScene())
{

}

Font::~Font()
{

}

bool Font::Load(const string& fileName)
{
	// We support these font sizes
	vector<int> fontSizes = {
		8, 9,
		10, 11, 12, 14, 16, 18,
		20, 22, 24, 26, 28,
		30, 32, 34, 36, 38,
		40, 42, 44, 46, 48,
		52, 56,
		60, 64, 68,
		72
	};

	for (auto& size : fontSizes)
	{
		TTF_Font* font = TTF_OpenFont(fileName.c_str(), size);
		if (font == nullptr)
		{
			SDL_Log("Failed to load font %s in size %d", fileName.c_str(), size);
			
			// ← ここで既に読み込んだフォントを解放
			for (auto& pair : mFontData)
			{
				TTF_CloseFont(pair.second);
			}
			mFontData.clear();

			return false;
		}
		mFontData.emplace(size, font);
	}
	return true;
}

void Font::Unload()
{
	for (auto& font : mFontData)
	{
		TTF_CloseFont(font.second);
	}
}

Texture* Font::RenderFreeText(const string& textKey, const Vector3& color, int pointSize)
{
	Texture* texture = nullptr;

	// Convert to SDL_Color
	SDL_Color sdlColor;
	sdlColor.r = static_cast<Uint8>(color.x * 255);
	sdlColor.g = static_cast<Uint8>(color.y * 255);
	sdlColor.b = static_cast<Uint8>(color.z * 255);
	sdlColor.a = 255;

	// Find the font data for this point size
	auto iter = mFontData.find(pointSize);
	if (iter != mFontData.end())
	{
		TTF_Font* font = iter->second;
		const string& actualText = textKey;
		// Draw this to a surface (blended for alpha)
		SDL_Surface* pixelsSurf = TTF_RenderText_Blended(font, actualText.c_str(), actualText.length(), sdlColor);
		if (!pixelsSurf) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to render text: %s", SDL_GetError());
			return nullptr;
		}

		// ピクセルフォーマットを RGBA32 に変換
		SDL_Surface* surf = SDL_ConvertSurface(pixelsSurf, SDL_PIXELFORMAT_RGBA32);
		// 元のサーフェスは不要なので削除
		SDL_DestroySurface(pixelsSurf);

		if (!surf) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to convert surface format: %s", SDL_GetError());
			return nullptr;
		}

		//SDL_Log("Rendering text: %s", actualText.c_str());
		if (surf != nullptr)
		{
			// Convert from surface to texture
			texture = new Texture();
			texture->CreateFromSurface(surf);
			SDL_DestroySurface(surf);
		}
	}
	else
	{
		SDL_Log("Point size %d is unsupported", pointSize);
	}

	return texture;
}
