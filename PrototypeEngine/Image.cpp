#include "Image.h"
#include "EngineWindow.h"
#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"


Image::Image(Entity* owner,int function)
	:Component(owner)
	,mTexture(nullptr)
	, mFilePath("")
	, mTextureRect()
	, mFillAmount(0)
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

	// RectTransformから「本来のサイズ」と「トランスフォーム情報」を取得
	auto* rt = mUIActor->GetRectTransform();
	float baseWidth = rt->GetRectScaleWidth();
	float baseHeight = rt->GetRectScaleHeight();
	// 描画用のサイズとオフセットの初期値
	float drawWidth = baseWidth;
	float drawHeight = baseHeight;
	float offsetX = 0;
	float offsetY = 0;

	if (mFillMethod == FillMethod::Horizontal) {
		drawWidth = baseWidth * mFillAmount;
		// 左端を固定（中心基準の矩形なので、縮んだ分の半分だけ左にずらす）
		offsetX = -(baseWidth - drawWidth);
	}
	else if (mFillMethod == FillMethod::Vertical) {
		drawHeight = baseHeight * mFillAmount;
		// 下端を固定（中心基準の矩形なので、縮んだ分の半分だけ下にずらす）
		offsetY = -(baseHeight - drawHeight);
	}


	Matrix4 pixelScale = Matrix4::CreateScale(
		drawWidth * rt->GetLocalScale().x,
		drawHeight * rt->GetLocalScale().y,
		1.0f
	);

	Matrix4 localRotate = Matrix4::CreateFromQuaternion(rt->GetLocalRotation());

	Matrix4 localTranslate = Matrix4::CreateTranslation(
		Vector3(rt->GetLocalPosition().x + offsetX, rt->GetLocalPosition().y + offsetY, 0.0f)
	);

	Matrix4 drawTransform = pixelScale * localRotate * localTranslate;

	if (rt->GetParentActor()) {
		drawTransform = drawTransform * rt->GetParentActor()->GetRectTransform()->GetWorldTransform();
	}

	shader->SetMatrixUniform("uWorldTransform", drawTransform);
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
	}
	else if (mFillMethod == FillMethod::Vertical)
	{
		// UV（v1を上にずらす）
		float filledV1 = v1 + (v2 - v1) * (1.0f - mFillAmount);
		uv.x = u1;
		uv.y = filledV1;
		uv.z = u2 - u1;
		uv.w = v2 - filledV1;
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

	ImGui::NewLine();

	//FillMethodタグのコンボUI
	if (ImGui::BeginCombo("FillMethod", ImageInformation::GetFillMethodName(mFillMethod).c_str()))
	{
		for (uint32_t i = 0; i < 4; ++i) {
			Image::FillMethod tag = static_cast<Image::FillMethod>(i);
			bool isSelected = (mFillMethod == tag);

			if (ImGui::Selectable(ImageInformation::GetFillMethodName(tag).c_str(), isSelected))
			{
				mFillMethod = tag;
			}
		}
		ImGui::EndCombo();
	}

	ImGui::NewLine();

	ImGui::Text("FillAmount");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50);
	ImGui::DragFloat("##fillAmount", &mFillAmount, 0.01f, 0.0f, 1.0f);

	ImGui::NewLine();

	ImGui::PopID();
}

Component* Image::Clone(Entity* newOwner) const
{
	Image* clone = new Image(newOwner);

	clone->mTexture = this->mTexture;
	clone->mFilePath = this->mFilePath;
	clone->mTextureRect = this->mTextureRect;
	clone->mFillAmount = this->mFillAmount;
	clone->mFillMethod = this->mFillMethod;
	clone->mVerticesCount = this->mVerticesCount;
	clone->mUVTransform = this->mUVTransform;
	
	return clone;
}