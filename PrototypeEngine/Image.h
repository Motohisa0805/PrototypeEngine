#pragma once
#include "Math.h"
#include "Texture.h"
#include "Shader.h"
#include "GameWinMain.h"
#include "BaseScene.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//画像描画をカプセル化したクラス
// UnityのImageに近いクラス
class Image
{
public:
	// UIがアクティブか閉じているかを追跡します
	enum UIState
	{
		EActive,
		EClosing,
		EDestroy
	};

	enum FillType
	{
		Simple,
		Filled
	};

	enum FillMethod
	{
		None,
		Horizontal,
		Vertical,
		Radial360
	};
protected:
	class BaseScene*		mGame;
	//画像
	class Texture*			mTexture;
	// Configure positions
	Vector2					mTexturePos;

	float					mOffsetX;
	float					mOffsetY;

	//スケーリング
	Vector3					mTexScale;

	float					mRectScaleWidth;
	float					mRectScaleHeight;

	float					mAngleZ;

	SDL_Rect				mTextureRect;

	float					mFillAmount;

	// State
	UIState					mState;

	FillType				mFillType;

	FillMethod				mFillMethod;

	int						mVerticesCount;

	vector<Image*>			mImages;
public:
							Image(int function = 0);
	virtual					~Image();
	//読み込み処理
	virtual void			Load(string file);
	virtual void			Update(float deltaTime);
	virtual void			Draw(class Shader* shader);
	// テクスチャを描くための関数
	void					DrawTexture(class Shader* shader);

	void					FillMethodCalculation(Vector4& uv,int& verticesCount);

	void					WorldMatrixCalculation(Matrix4& trans,Matrix4& rotate,Matrix4& scale);

	virtual void			UnLoad();
	// 状態を閉鎖に設定
	void					Close();

	void					Active();

	//***Getter***
	virtual SDL_Rect		GetSDL_Rect() { return mTextureRect; }
	
	virtual float			GetFillAmount() { return mFillAmount; }
	// UI画面の状態を取得する
	UIState					GetState() const 
	{
		return mState; 
	}

	class Texture*			GetTexture() const { return mTexture; }

	//***Setter***
	//画像を設定
	virtual void			SetTexture(class Texture* texture);
	//画像の位置を設定
	virtual void			SetPosition(Vector2 pos);
	//画像のスケーリングを設定
	virtual void			SetScale(Vector3 scale);
	virtual void			SetSDL_Rect(SDL_Rect rect) { mTextureRect = rect; }
	virtual void			SetFillAmount(float fill);
	//画像の回転を行う
	virtual void			SetAngleZ(float angle);

	virtual void			SetFillType(FillType type) { mFillType = type; }

	virtual void			SetFillMethod(FillMethod method) { mFillMethod = method; }

	virtual FillMethod		GetFillMethod() { return mFillMethod; }

	virtual void			SetVerticesCount(int count) { mVerticesCount = count; }
	virtual int				GetVerticesCount() { return mVerticesCount; }

	virtual void 			SetState(UIState state);

	void					AddChildUIImage(Image* image);
};