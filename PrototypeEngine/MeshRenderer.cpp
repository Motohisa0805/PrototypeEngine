#include "MeshRenderer.h"


MeshRenderer::MeshRenderer(ActorObject* owner, bool isSkeletal)
	:Component(owner)
	, mVisible(true)
	, mIsSkeletal(isSkeletal)
	, mFilePath("")
	, mAlpha(1.0f)
{
	mName = "MeshRenderer";
	EngineWindow::GetRenderer()->AddMeshComp(this);
}

MeshRenderer::~MeshRenderer()
{
	EngineWindow::GetRenderer()->RemoveMeshComp(this);
}

void MeshRenderer::Draw(Shader* shader)
{
	for (unsigned int i = 0; i < mMeshs.size(); i++) 
	{
		for (unsigned int j = 0; j < mMeshs[i]->GetVertexArrays().size(); j++)
		{
			if (mMeshs[i])
			{
				// Set the world transform
				shader->SetMatrixUniform("uWorldTransform",
					mOwner->GetWorldTransform());
				Texture* t = nullptr;
				// Set the active texture
				t = mMeshs[i]->GetTexture(j);
				if (t)
				{
					t->SetActive();
				}
				else {
					shader->SetNoTexture();
				}
				MaterialInfo m = mMeshs[i]->GetMaterialInfo()[j];

				// 不透明度によってブレンド設定（1回だけで済むならループの外でもOK）
				if (m.Color.w < 1.0f)
				{
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glDepthMask(GL_FALSE);  // 透明物体は深度書き込み無効（任意）
				}
				else
				{
					glDisable(GL_BLEND);
					glDepthMask(GL_TRUE);   // 不透明物体は通常通り
				}

				shader->SetColorUniform("uTexture", m);
				// メッシュの頂点配列をアクティブに設定します
				VertexArray* va = mMeshs[i]->GetVertexArrays()[j];
				va->SetActive();
				// 描画
				glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
			}
		}
	}
}

void MeshRenderer::DrawForShadowMap(Shader* shader)
{
	for (unsigned int i = 0; i < mMeshs.size(); i++)
	{
		for (unsigned int j = 0; j < mMeshs[i]->GetVertexArrays().size(); j++)
		{
			if (!mMeshs[i]) continue;

			// ワールド変換のみ設定
			shader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());

			// ブレンドなどはシャドウマップ描画時は一切不要
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);

			// 頂点配列をアクティブに
			VertexArray* va = mMeshs[i]->GetVertexArrays()[j];
			va->SetActive();

			// 描画
			glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

void MeshRenderer::SetMaterialAlpha(float alpha)
{
	float a = Math::Clamp(alpha, 0.0f, 1.0f);
	if (!mMeshs.empty())
	{
		for (auto& mesh : mMeshs)
		{
			vector<MaterialInfo> info = mesh->GetMaterialInfo();
			for (int i = 0; i < info.size(); ++i)
			{
				info[i].Color = Vector4(info[i].Color.x, info[i].Color.y, info[i].Color.z, a);
			}
			mesh->SetMaterialInfo(info);
		}
	}
}

void MeshRenderer::Serialize(json& j) const
{
	Component::Serialize(j);
	// ロード元のファイルパスをそのままJSONに書き込む
	j["FilePath"] = mFilePath;

	// メッシュレンダラー固有の他のプロパティも追加
	j["Visible"] = mVisible;
	j["IsSkeletal"] = mIsSkeletal;
	j["Alpha"] = mAlpha;
}

void MeshRenderer::Deserialize(const json& j)
{
	Component::Deserialize(j);
	//モデルパスがあるなら
	if (j.contains("FilePath"))
	{
		// 1. JSONからファイルパスを取得する
		std::string filePath = j.at("FilePath").get<std::string>();

		// 2. メンバ変数にファイルパスを設定
		mFilePath = filePath;

		// 3. ファイルパスを使って、Rendererからメッシュをロードし、設定する
		//    元のコードにあった処理をここで実行します
		vector<class Mesh*> mesh = EngineWindow::GetRenderer()->GetMeshs(mFilePath);
		AddMeshs(mesh);
	}

	// 4. その他のプロパティも読み込む
	if (j.contains("Visible"))
	{
		mVisible = j.at("Visible").get<bool>();
	}
	if (j.contains("IsSkeletal"))
	{
		mIsSkeletal = j.at("IsSkeletal").get<bool>();
	}
	if (j.contains("Alpha"))
	{
		mAlpha = j.at("Alpha").get<float>();
		SetMaterialAlpha(mAlpha);
	}
}

void MeshRenderer::DrawGUI()
{
	//MeshRendererのプロパティ
	ImGui::Text("Mesh Renderer Properties");

	//1.ファイルパスの取得
	string currentPath = mFilePath;
	static char pathBuffer[256];
	strncpy_s(pathBuffer, currentPath.c_str(), sizeof(pathBuffer));
	pathBuffer[sizeof(pathBuffer) - 1] = '\0';
	ImGui::NewLine();
	ImGui::Text("FilePath DragDropTarget");
	//2.ファイルパスの入力フィールド
	ImGui::InputText("Mesh File Path", pathBuffer, sizeof(pathBuffer), ImGuiInputTextFlags_ReadOnly);

	//3.ファイルロードボタン(ここでファイル選択UIを開くか、ProjectPanelからのDrag&Dropを想定)
	//Drag&Drop想定
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			//ペイロードがファイルパスであると仮定
			const char* dropPath = (const char*)payload->Data;
			string path = StringConverter::ExtensionFileName(dropPath);
			//ファイルパスを使いロード処理を呼び出す
			vector<class Mesh*> mesh = EngineWindow::GetRenderer()->GetMeshs(path);
			SetMeshs(mesh);
			mAlpha = mesh[0]->GetMaterialInfo()[0].Color.w;
			mFilePath = path;
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::NewLine();

	if (!mMeshs.empty())
	{
		ImGui::Text("Alpha Setting");
		ImGui::SliderFloat("Alpha", &mAlpha, 0.0f, 1.0f,"%.2f");
		ImGui::SetNextItemWidth(50);
		ImGui::DragFloat("Alpha Input", &mAlpha, 1.0f, 0.0f, 1.0f, "%.2f");

		if (mAlpha != mMeshs[0]->GetMaterialInfo()[0].Color.w)
		{
			SetMaterialAlpha(mAlpha);
		}
	}

	ImGui::Separator();
}
