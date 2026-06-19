#pragma once
#include "Component.h"
#include "Math.h"
#include "Texture.h"
#include "UIActor.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class Shader;


//画像描画をカプセル化したクラス
// UnityのImageに近いクラス
class Image : public Component
{
public:
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

	string 					mFilePath;

	float					mFillAmount;

	FillMethod				mFillMethod;

	
	SDL_Rect				mTextureRect;


	int						mVerticesCount;

	Vector4					mUVTransform;
public:
							Image(Entity* owner,int function = 0);
	virtual					~Image();
	//読み込み処理
	virtual void			Load(string file);
	virtual void			Update(float deltaTime);
	void					OnUpdateWorldTransform()override;
	virtual void			Draw(Shader* shader);
	// テクスチャを描くための関数
	void					DrawTexture(Shader* shader);

	void					FillMethodCalculation(Vector4& uv,int& verticesCount);

	virtual void			UnLoad();

	//***Getter***
	virtual SDL_Rect		GetSDL_Rect() { return mTextureRect; }
	
	virtual float			GetFillAmount() { return mFillAmount; }

	Texture*				GetTexture() const { return mTexture; }

	//***Setter***
	//画像を設定
	virtual void			SetTexture(Texture* texture);
	virtual void			SetSDL_Rect(SDL_Rect rect) { mTextureRect = rect; }
	virtual void			SetFillAmount(float fill);

	virtual void			SetFillMethod(FillMethod method) { mFillMethod = method; }

	virtual FillMethod		GetFillMethod() { return mFillMethod; }

	virtual void			SetVerticesCount(int count) { mVerticesCount = count; }
	virtual int				GetVerticesCount() { return mVerticesCount; }

	void					SetIsRun(bool run)override;

	void					Serialize(json& j) const override;
	void					Deserialize(const json& j)override;

	void					DrawCustomGUI(const std::vector<PropertyInfo>& properties)override;

	Component*				Clone(Entity* newOwner) const override;
};

namespace ImageInformation {
	inline string GetFillMethodName(Image::FillMethod tag)
	{
		switch (tag)
		{
		case Image::FillMethod::None:
			return "None";
		case Image::FillMethod::Horizontal:
			return "Horizontal";
		case Image::FillMethod::Vertical:
			return "Vertical";
		case Image::FillMethod::Radial360:
			return "Radial360";
		default:
			break;
		}
	}
};
