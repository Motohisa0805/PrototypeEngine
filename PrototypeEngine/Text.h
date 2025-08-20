#pragma once
#include "Image.h"
#include "Font.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

// テキストを表示するためのクラス
// 主にフォントとテキストの内容を管理します。
class Text : public Image
{
private:
	// テキストの名前
	string			mName;
	// UTF-8形式のテキスト内容
	ccp_String      mText;
	// フォント
	class Font*		mFont;
	// テキストの位置
	Vector2			mDimensions;
	
	bool			mHighlighted;
	// テキストのフォントサイズ
	int				mFontSize;
	// テキストの色
	Vector3			mColor;
public:
					Text(class Font* font,const Vector2& pos,int function = 0);
					~Text();

	// Set the name of the button
	void			SetText(const string& name);

	void			SetUTF_8Text(const char8_t* name);

	void			SetFontSize(int size);

	void			SetColor(Vector3 color);


	class Font*		GetFont()const { return mFont; }
};