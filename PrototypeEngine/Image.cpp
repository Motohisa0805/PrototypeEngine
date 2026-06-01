#include "Image.h"
#include "SceneManager.h"
#include "EngineWindow.h"
#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"
#include "BaseScene.h"

Image::Image(Entity* owner,int function)
	:Component(owner)
	,mTexture(nullptr)
	, mFilePath("")
	, mTextureRect()
	, mFillAmount(0)
	, mFillType(FillType::Simple)
	, mFillMethod(FillMethod::None)
	, mUVTransform(Vector4(0, 0, 1, 1))
	, mVerticesCount(6)
{
	mName = "Image";

	mHeaderColor = Vector4(0.4f, 0.4f, 0.8f, 1.0f);
	mHeaderHoveredColor = Vector4(0.3f, 0.3f, 0.6f, 1.0f);
	mHeaderActiveColor = Vector4(0.4f, 0.4f, 0.8f, 1.0f);

	mFillAmount = 1.0f;
	mFillMethod = FillMethod::None;
	EngineWindow::GetRenderer()->AddImageComps(this);
}

Image::~Image()
{
	mTexture = nullptr;
	mFilePath = "";
	EngineWindow::GetRenderer()->RemoveImageComp(this);
}

void Image::Load(string file)
{
	string filePath = file;
	mTexture = EngineWindow::GetRenderer()->GetTexture(filePath);
	mTextureRect.x = 0;
	mTextureRect.y = 0;
	mTextureRect.w = static_cast<float>(mTexture->GetWidth());
	mTextureRect.h = static_cast<float>(mTexture->GetHeight());
	mUIActor->GetRectTransform()->SetScaleWidthAndHeight(static_cast<float>(mTextureRect.w), static_cast<float>(mTextureRect.h));
	//読み込んだ時に一度画像の描画の描画の形を設定
	FillMethodCalculation(mUVTransform, mVerticesCount);
}

void Image::SetTexture(Texture* texture)
{
	mTexture = texture;
	mTextureRect.x = 0;
	mTextureRect.y = 0;
	mTextureRect.w = static_cast<float>(mTexture->GetWidth());
	mTextureRect.h = static_cast<float>(mTexture->GetHeight());
}

void Image::SetFillAmount(float fill)
{
	mFillAmount = Math::Clamp(fill, 0.0f, 1.0f);
}

void Image::Update(float deltaTime)
{
}

void Image::OnUpdateWorldTransform()
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

void Image::DrawTexture(Shader* shader)
{
	//画像の描画の描画の形を設定
	FillMethodCalculation(mUVTransform, mVerticesCount);

	shader->SetVector4Uniform("uTexUV", mUVTransform);
	
	shader->SetMatrixUniform("uWorldTransform", mUIActor->GetRectTransform()->GetDrawTransform());

	mTexture->SetActive();
	
	glDrawElements(GL_TRIANGLES, mVerticesCount, GL_UNSIGNED_INT, nullptr);
}

void Image::FillMethodCalculation(Vector4& uv, int& verticesCount)
{
	if (!mTexture) { return; }
	uv = Vector4(0, 0, 1, 1);

	// UV範囲の計算
	float u1 = mTextureRect.x / mTexture->GetWidth();
	float v1 = mTextureRect.y / mTexture->GetHeight();
	float u2 = (mTextureRect.x + mTextureRect.w) / mTexture->GetWidth();
	float v2 = (mTextureRect.y + mTextureRect.h) / mTexture->GetHeight();



	// 横幅をmFillAmountでスケール
	if (mFillMethod == FillMethod::Horizontal)
	{
		float filledU2 = u1 + (u2 - u1) * mFillAmount;
		uv.x = u1;
		uv.y = v1;
		uv.z = filledU2 - u1;
		uv.w = v2 - v1;

		// 横幅をmFillAmountでスケール
		mUIActor->GetRectTransform()->SetScaleWidth(mUIActor->GetRectTransform()->GetRectScaleWidth() * mFillAmount);

		// 左端を固定して右に伸びるように位置補正（中心基準からオフセット）
		float offsetX = (1.0f - mFillAmount) * 0.5f * mTextureRect.w * mUIActor->GetRectTransform()->GetScale().x;
		mUIActor->GetRectTransform()->SetOffsetY(offsetX);
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
		mUIActor->GetRectTransform()->SetScaleHeight(mUIActor->GetRectTransform()->GetRectScaleHeight() * mFillAmount);

		// Y方向の位置補正（下から上に伸びるので上にずらす）
		float offsetY = (1.0f - mFillAmount) * 0.5f * mTexture->GetHeight();
		mUIActor->GetRectTransform()->SetOffsetY(offsetY);
	}
	else if (mFillMethod == FillMethod::Radial360)
	{
		verticesCount = mVerticesCount;
	}
}

void Image::SetIsRun(bool run)
{
	Component::SetIsRun(run);
	if (run) {
		mOwner->SetState(Entity::State::EActive);
	}
	else {
		mOwner->SetState(Entity::State::EPaused);
	}
}

void Image::Serialize(json& j) const
{
	Component::Serialize(j);

	// ロード元のファイルパスをそのままJSONに書き込む
	j["FilePath"] = mFilePath;

	j["FillAmount"] = mFillAmount;
	j["FillType"] = mFillType;
	j["FillMethod"] = mFillMethod;
	j["VerticesCount"] = mVerticesCount;
	j["UVTransform"] = { mUVTransform.x,mUVTransform.y,mUVTransform.z,mUVTransform.w };
}

void Image::Deserialize(const json& j)
{
	Component::Deserialize(j);
	if (j.contains("FilePath")) {
		// 1. JSONからファイルパスを取得する
		std::string filePath = j.at("FilePath").get<std::string>();

		// 2. メンバ変数にファイルパスを設定
		mFilePath = filePath;

		Load(mFilePath);
	}
	if (j.contains("FillAmount")) {
		mFillAmount = j.at("FillAmount").get<float>();
	}
	if (j.contains("FillType")) {
		mFillType = j.at("FillType").get<FillType>();
	}
	if (j.contains("FillMethod")) {
		mFillMethod = j.at("FillMethod").get<FillMethod>();
	}
	if (j.contains("VerticesCount")) {
		mVerticesCount = j.at("VerticesCount").get<int>();
	}
	if (j.contains("UVTransform")) {
		mUVTransform = Vector4
		(
			j["UVTransform"][0],
			j["UVTransform"][1],
			j["UVTransform"][2],
			j["UVTransform"][3]
		);
	}
}

void Image::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
	ImGui::PushID(this);
	//1.ファイルパスの取得
	string currentPath = mFilePath;
	static char pathBuffer[256];
	strncpy_s(pathBuffer, currentPath.c_str(), sizeof(pathBuffer));
	pathBuffer[sizeof(pathBuffer) - 1] = '\0';
	ImGui::Text("FilePath DragDropTarget");
	//2.ファイルパスの入力フィールド
	ImGui::InputText("Image File Path", pathBuffer, sizeof(pathBuffer), ImGuiInputTextFlags_ReadOnly);
	//3.ファイルロードボタン(ここでファイル選択UIを開くか、ProjectPanelからのDrag&Dropを想定)
	//Drag&Drop想定
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			//ペイロードがファイルパスであると仮定
			const char* dropPath = (const char*)payload->Data;
			Load(dropPath);
			mFilePath = dropPath;
			mUIActor->GetRectTransform()->SetDirty();
		}
		ImGui::EndDragDropTarget();
	}
	if (ImGui::Button("Clear Image"))
	{
		mTexture = nullptr;
		mFilePath = "";
	}

	ImGui::PopID();
}

Component* Image::Clone(Entity* newOwner) const
{
	Image* clone = new Image(newOwner);

	clone->mTexture = this->mTexture;
	clone->mFilePath = this->mFilePath;
	clone->mTextureRect = this->mTextureRect;
	clone->mFillAmount = this->mFillAmount;
	clone->mFillType = this->mFillType;
	clone->mFillMethod = this->mFillMethod;
	clone->mVerticesCount = this->mVerticesCount;
	clone->mUVTransform = this->mUVTransform;
	
	return clone;
}