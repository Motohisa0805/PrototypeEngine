#pragma once
#include "Component.h"
#include "Math.h"
#include "Texture.h"
#include "UIActor.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class BaseScene;
class Shader;


//画像描画をカプセル化したクラス
// UnityのImageに近いクラス
class Image : public Component
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
	//画像
	Texture*				mTexture;

	float					mOffsetX;
	float					mOffsetY;

	float					mRectScaleWidth;
	float					mRectScaleHeight;

	SDL_Rect				mTextureRect;

	float					mFillAmount;

	// State
	UIState					mState;

	FillType				mFillType;

	FillMethod				mFillMethod;

	int						mVerticesCount;
public:
							Image(Entity* owner,int function = 0);
	virtual					~Image();
	//読み込み処理
	virtual void			Load(string file);
	virtual void			Update(float deltaTime);
	virtual void			Draw(Shader* shader);
	// テクスチャを描くための関数
	void					DrawTexture(Shader* shader);

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

	Texture*				GetTexture() const { return mTexture; }

	//***Setter***
	//画像を設定
	virtual void			SetTexture(Texture* texture);
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
};