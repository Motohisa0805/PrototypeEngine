#include "SkeletalMeshRenderer.h"


SkeletalMeshRenderer::SkeletalMeshRenderer(ActorObject* owner)
	:MeshRenderer(owner, true)
	, mSkeleton(nullptr)
	, mAnimator(nullptr)
{
}

SkeletalMeshRenderer::~SkeletalMeshRenderer()
{
}

void SkeletalMeshRenderer::Draw(Shader* shader)
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
				// Set the matrix palette
				shader->SetMatrixUniforms("uMatrixPalette",
					&mAnimator->GetPalette().mEntry[0],
					SkeletonLayout::MAX_SKELETON_BONES);
				Texture* t = nullptr;
				// Set the active texture
				t = mMeshs[i]->GetTexture(j);
				if (t)
				{
					t->SetActive();
				}
				else 
				{
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
				//マテリアルの色を設定
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

void SkeletalMeshRenderer::DrawForShadowMap(Shader* shader)
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
				// Set the matrix palette
				shader->SetMatrixUniforms("uMatrixPalette",
					&mAnimator->GetPalette().mEntry[0],
					SkeletonLayout::MAX_SKELETON_BONES);

				// ブレンドなどはシャドウマップ描画時は一切不要
				glDisable(GL_BLEND);
				glDepthMask(GL_TRUE);

				// メッシュの頂点配列をアクティブに設定します
				VertexArray* va = mMeshs[i]->GetVertexArrays()[j];
				va->SetActive();
				// 描画
				glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
			}
		}
	}
}

void SkeletalMeshRenderer::Update(float deltaTime)
{
	/*
	if (!mAnimator) { return; }
	mAnimator->Update(deltaTime);
	*/
}
void SkeletalMeshRenderer::LoadSkeletonMesh(const string& fileName, ActorObject* actor)
{
	const vector<class Mesh*>& mesh = EngineWindow::GetRenderer()->GetMeshs(fileName);
	mMeshs.insert(mMeshs.end(), mesh.begin(), mesh.end());

	Skeleton* sk = mGame->GetSkeleton(fileName);
	mSkeleton = sk;
	if (mSkeleton != nullptr)
	{
		mSkeleton->SetParentActor(actor);
	}
	mIsSkeletal = true;
}

void SkeletalMeshRenderer::SetAnimator(Animator* animator)
{
	if (animator == nullptr)
	{
		Debug::ErrorLog("The project is ending because there are no Animator.");
		return;
	}
	mAnimator = animator;
}
