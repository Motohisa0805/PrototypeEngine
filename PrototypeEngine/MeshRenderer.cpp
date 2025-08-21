#include "MeshRenderer.h"


MeshRenderer::MeshRenderer(ActorObject* owner, bool isSkeletal)
	:Component(owner)
	, mTextureIndex(0)
	, mVisible(true)
	, mIsSkeletal(isSkeletal)
{
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
