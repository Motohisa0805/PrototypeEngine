#pragma once
#include "Math.h"
#include "Texture.h"
#include "Shader.h"
#include "GameWinMain.h"
#include "Font.h"
#include "Image.h"
#include "Text.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class Text;

class Font;

//書籍元を改造したファイル
//ボタンUIのクラス
// ボタンは画像とテキストを持ち、クリックイベントを処理します
class Button : public Image
{
public:
	Button(const string& name, Font* font,
		std::function<void()> onClick,
		const Vector2& pos, const Vector2& dims);
	Button(const char8_t* name, Font* font,
		std::function<void()> onClick,
		const Vector2& pos, const Vector2& dims);
	~Button();

	void					Update(float deltaTime)override;

	void					SetButtonText(Texture* texture);
	// Getters/setters
	Text*					GetNameText() { return mNameText; }
	const Vector2&			GetPosition() const { return mPosition; }
	void					SetHighlighted(bool sel) { mHighlighted = sel; }
	bool					GetHighlighted() const { return mHighlighted; }

	// ポイントがボタンの境界内にある場合はtrueを返します
	bool					ContainsPoint(const Vector2& pt) const;
	// ボタンがクリックされたときに呼び出されます
	void					OnClick();

	Coroutine				DicideButton();
private:
	std::function<void()>	mOnClick;

	float					mClickCount;

	float					mDicideButton;

	string					mName;

	Image*					mButtonImage;

	Text*					mNameText;

	Font*					mFont;

	Vector2					mPosition;

	Vector2					mDimensions;

	bool					mHighlighted;

	Vector2					mBaseButtonPosition;
};