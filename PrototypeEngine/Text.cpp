#include "Text.h"

Text::Text(Font* font, const Vector2& pos, int function)
: Image(function)
, mFont(font)
, mHighlighted(false)
{
	mTexture = nullptr;
	mTexturePos = pos;
	mFontSize = 30;
	mColor = Color::White;
}

Text::~Text()
{
	delete mTexture;
}

void Text::SetText(const string& name)
{
	const char* text = name.c_str();
	mName = name;

	if (mTexture)
	{
		mTexture->Unload();
		delete mTexture;
		mTexture = nullptr;
	}
	mTexture = mFont->RenderFreeText(mName,mColor,mFontSize);
	mTextureRect.x = 0;
	mTextureRect.y = 0;
	mTextureRect.w = static_cast<float>(mTexture->GetWidth());
	mTextureRect.h = static_cast<float>(mTexture->GetHeight());
}

void Text::SetUTF_8Text(const char8_t* name)
{
	const char* text = reinterpret_cast<const char*>(name);
	mText = text;

	if (mTexture)
	{
		mTexture->Unload();
		delete mTexture;
		mTexture = nullptr;
	}
	mTexture = mFont->RenderFreeText(mText, mColor, mFontSize);
	mTextureRect.x = 0;
	mTextureRect.y = 0;
	mTextureRect.w = static_cast<float>(mTexture->GetWidth());
	mTextureRect.h = static_cast<float>(mTexture->GetHeight());
}

void Text::SetFontSize(int size)
{
	mFontSize = size;
}

void Text::SetColor(Vector3 color)
{
	mColor = color;
}
