#include "Button.h"
#include "GameWinMain.h"
#include "Font.h"
#include "Text.h"
#include "Texture.h"
#include "CallBackManager.h"
#include "BaseScene.h"

Button::Button(const string& name, Font* font,
	std::function<void()> onClick,
	const Vector2& pos, const Vector2& dims)

	: Image()
	, mOnClick(onClick)
	, mClickCount(0)
	, mFont(font)
	, mPosition(pos)
	, mDimensions(dims)
	, mHighlighted(false)
{
	mBaseButtonPosition = pos;

	mButtonImage = new Image();
	mButtonImage->SetPosition(pos);

	mNameText = new Text(mFont,mPosition);
	mNameText->SetText(name);
}

Button::Button(const char8_t* name, Font* font, std::function<void()> onClick, const Vector2& pos, const Vector2& dims)
	: Image()
	, mOnClick(onClick)
	, mClickCount(0)
	, mFont(font)
	, mPosition(pos)
	, mDimensions(dims)
	, mHighlighted(false)
{
	mBaseButtonPosition = pos;

	mButtonImage = new Image();
	mButtonImage->Load("ButtonBlue.png");
	mButtonImage->SetPosition(pos);

	mNameText = new Text(mFont, mPosition);
	mNameText->SetUTF_8Text(name);
}

Button::~Button()
{

	mGame->RemoveImage(this);
	//ボタンの枠
	if (mButtonImage)
	{
		mGame->RemoveImage(mButtonImage);
		delete mButtonImage;
	}
	if (mNameText)
	{
		//ボタンの文字
		mGame->RemoveImage(mNameText);
		delete mNameText;
	}
}

void Button::Update(float deltaTime)
{
	if (GameStateClass::gGameEventFrag && mDicideButton > 0)
	{
		mDicideButton -= Time::gUnscaledDeltaTime;
		if (mDicideButton < 0)
		{
			mButtonImage->SetPosition(mBaseButtonPosition);

			mNameText->SetPosition(mBaseButtonPosition);
		}
	}

	if (GameStateClass::gGameEventFrag&&mClickCount > 0)
	{
		mClickCount -= Time::gUnscaledDeltaTime;
		if (mClickCount < 0)
		{
			GameStateClass::gGameEventFrag = false;
			if (mOnClick)
			{
				mOnClick();
			}
		}
	}
}

void Button::SetButtonText(Texture* texture)
{
	mButtonImage->SetTexture(texture);
}

bool Button::ContainsPoint(const Vector2& pt) const
{
	bool no = pt.x < (mPosition.x - mDimensions.x / 2.0f) ||
		pt.x >(mPosition.x + mDimensions.x / 2.0f) ||
		pt.y < (mPosition.y - mDimensions.y / 2.0f) ||
		pt.y >(mPosition.y + mDimensions.y / 2.0f);
	return !no;
}

void Button::OnClick()
{
	if (GameStateClass::gGameEventFrag) { return; }


	GameStateClass::gGameEventFrag = true;

	Vector2 pos = mBaseButtonPosition;
	pos.y -= 5.0f;
	mButtonImage->SetPosition(pos);

	mNameText->SetPosition(pos);

	mDicideButton = 0.1f;

	mClickCount = 0.2f;

	//DicideButton();
}

Coroutine Button::DicideButton()
{
	Vector2 pos = mBaseButtonPosition;
	pos.y -= 5.0f;
	mButtonImage->SetPosition(pos);

	mNameText->SetPosition(pos);

	mDicideButton = 0.1f;

	mClickCount = 0.2f;
	
	co_await WaitForSeconds{ std::chrono::milliseconds(100) };

	mButtonImage->SetPosition(mBaseButtonPosition);

	mNameText->SetPosition(mBaseButtonPosition);

}
