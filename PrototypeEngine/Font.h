#pragma once

#include "Math.h"
#include "Texture.h"
#include "BaseScene.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class BaseScene;

//書籍元を一部改造したファイル
//文字のフォントを読み込み、参照するクラス
// フォントのポイントサイズごとにTTF_Fontを管理するクラス
class Font
{
private:
	// ポイントサイズとフォントデータのマップ
	std::unordered_map<int, TTF_Font*>	mFontData;
	BaseScene*							mGame;
public:
										Font();
										~Font();

	// ファイルからのロード/アンロード
	bool								Load(const string& fileName);
	void								Unload();

	// 与えられた文字列とフォントを使用して、テクスチャに描画する
	//上記との違いは自由に引数で取得した文字を取得できる所
	class								Texture* RenderFreeText(const string& textKey,
										const Vector3& color = Color::White,
										int pointSize = 30);
};