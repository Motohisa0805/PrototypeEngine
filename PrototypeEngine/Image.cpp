#include "Image.h"


Image::Image(int function)
	:mGame(GameApp::GetActiveScene())
	,mTexture(nullptr)
	,mAngleZ(0)
{
	mTexScale = Vector3(1.0f, 1.0f, 1.0f);
	mFillAmount = 1.0f;
	mFillMethod = FillMethod::None;
	if (function == Release_Function)
	{
		//BaseSceneに送る処理
		mGame->PushImage(this);
	}
	else
	{
		mGame->PushDebugImage(this);
	}
}

Image::~Image()
{
	mState = EDestroy;
}

void Image::Load(string file)
{
	string filePath = TexFile::TextureFilePath + file;
	mTexture = GameWinMain::GetRenderer()->GetTexture(filePath);
	mTextureRect.x = 0;
	mTextureRect.y = 0;
	mTextureRect.w = static_cast<float>(mTexture->GetWidth());
	mTextureRect.h = static_cast<float>(mTexture->GetHeight());
}

void Image::SetTexture(Texture* texture)
{
	mTexture = texture;
	mTextureRect.x = 0;
	mTextureRect.y = 0;
	mTextureRect.w = static_cast<float>(mTexture->GetWidth());
	mTextureRect.h = static_cast<float>(mTexture->GetHeight());
}

void Image::SetPosition(Vector2 pos)
{
	mTexturePos = pos;
}

void Image::SetScale(Vector3 scale)
{
	mTexScale = scale;
}

void Image::SetFillAmount(float fill)
{
	mFillAmount = Math::Clamp(fill, 0.0f, 1.0f);
}

void Image::SetAngleZ(float angle)
{
	mAngleZ = angle;
}

void Image::SetState(UIState state)
{
	mState = state;

}

void Image::AddChildUIImage(Image* image)
{
	mImages.emplace_back(image);
}

void Image::Update(float deltaTime)
{
}

void Image::Draw(Shader* shader)
{
	// Draw title (if exists)
	if (mTexture)
	{
		DrawTexture(shader);
	}
}

void Image::UnLoad()
{
	if (mTexture) 
	{
		mTexture->Unload();
		delete mTexture;
	}
}

void Image::Close()
{
	mState = EClosing;
}

void Image::Active()
{
	mState = EActive;
}

void Image::DrawTexture(Shader* shader)
{
	Vector4 uvTransform = Vector4(0, 0, 1, 1);
	int verticesCount = 6;
	//画像の描画の描画の形を設定
	FillMethodCalculation(uvTransform,verticesCount);

	shader->SetVector4Uniform("uTexUV", uvTransform);

	Matrix4 scaleMat;

	Matrix4 transMat;

	Matrix4 rotationMat;

	WorldMatrixCalculation(transMat,rotationMat,scaleMat);

	Matrix4 world = scaleMat * rotationMat * transMat;
	
	shader->SetMatrixUniform("uWorldTransform", world);

	mTexture->SetActive();
	
	glDrawElements(GL_TRIANGLES, verticesCount, GL_UNSIGNED_INT, nullptr);
}

void Image::FillMethodCalculation(Vector4& uv, int& verticesCount)
{
	uv = Vector4(0, 0, 1, 1);

	// UV範囲の計算
	float u1 = mTextureRect.x / mTexture->GetWidth();
	float v1 = mTextureRect.y / mTexture->GetHeight();
	float u2 = (mTextureRect.x + mTextureRect.w) / mTexture->GetWidth();
	float v2 = (mTextureRect.y + mTextureRect.h) / mTexture->GetHeight();

	// 横幅をmFillAmountでスケール
	mRectScaleWidth = static_cast<float>(mTextureRect.w);
	mRectScaleHeight = static_cast<float>(mTextureRect.h);

	if (mFillMethod == FillMethod::Horizontal)
	{
		float filledU2 = u1 + (u2 - u1) * mFillAmount;
		uv.x = u1;
		uv.y = v1;
		uv.z = filledU2 - u1;
		uv.w = v2 - v1;

		// 横幅をmFillAmountでスケール
		mRectScaleWidth *= mFillAmount;

		// 左端を固定して右に伸びるように位置補正（中心基準からオフセット）
		mOffsetX = (1.0f - mFillAmount) * 0.5f * mTextureRect.w * mTexScale.x;
	}
	else if (mFillMethod == FillMethod::Vertical)
	{
		// UV（v1を上にずらす）
		float filledV1 = v1 + (v2 - v1) * (1.0f - mFillAmount);
		uv.x = u1;
		uv.y = filledV1;
		uv.z = u2 - u1;
		uv.w = v2 - filledV1;

		// スケーリング（高さをfillAmount倍）
		mRectScaleHeight *= mFillAmount;

		// Y方向の位置補正（下から上に伸びるので上にずらす）
		mOffsetY = (1.0f - mFillAmount) * 0.5f * mTexture->GetHeight();
	}
	else if (mFillMethod == FillMethod::Radial360)
	{
		verticesCount = mVerticesCount;
	}
}

void Image::WorldMatrixCalculation(Matrix4& trans, Matrix4& rotate, Matrix4& scale)
{
	scale = Matrix4::CreateScale(
		mRectScaleWidth * mTexScale.x,
		mRectScaleHeight * mTexScale.y,
		mTexScale.z);


	trans = Matrix4::CreateTranslation(
		Vector3(mTexturePos.x - mOffsetX, mTexturePos.y - mOffsetY, 0.0f));

	rotate = Matrix4::CreateRotationZ(mAngleZ);
}
