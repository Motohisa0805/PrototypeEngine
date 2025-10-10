#include "SkeletalMeshRenderer.h"


SkeletalMeshRenderer::SkeletalMeshRenderer(ActorObject* owner)
	:MeshRenderer(owner, true)
	, mSkeleton(nullptr)
	, mAnimator(nullptr)
{
	mName = "SkeletalMeshRenderer";
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

void SkeletalMeshRenderer::Serialize(json& j) const
{
	// 1. ベースクラス (MeshRenderer) のシリアライズを呼び出す
	//    -> これにより、mMeshFilePath やその他の基本プロパティが書き込まれる
	MeshRenderer::Serialize(j);

	// 2. コンポーネントの型を「SkeletalMeshRenderer」で上書き
	//    -> ActorObject::Deserialize()のファクトリー処理で、この型を使って
	//       SkeletalMeshRendererのインスタンスが生成されるようにする
	//j["Type"] = "SkeletalMeshRenderer";

	// 3. (必要に応じて) スケルタルメッシュ固有のプロパティを追記
	//    mAnimatorは通常、Actorの別コンポーネントとしてシリアライズされるため、ここでは省略
}

void SkeletalMeshRenderer::Deserialize(const json& j)
{
	// 1. ベースクラスのデシリアライズを呼び出す (MeshRenderer::Deserialize)
	//    -> JSONから mMeshFilePath を読み込み、EngineWindow::GetRenderer()->GetMeshs() を呼び出して
	//       mMeshs (メッシュ) のロードと設定を完了させる。
	MeshRenderer::Deserialize(j);

	// 2. ベースクラスで読み込まれたファイルパス (mMeshFilePath) を使ってスケルトンをロードする
	std::string fileName = GetMeshFilePath(); // GetMeshFilePath() が mMeshFilePath を返す前提

	// SkeletalMeshRenderer::LoadSkeletonMesh のスケルトンロード部分のロジック
	Skeleton* sk = mOwner->GetGame()->GetSkeleton(fileName);
	mSkeleton = sk;

	if (mSkeleton != nullptr)
	{
		mSkeleton->SetParentActor(mOwner); // Actorにスケルトンを設定
	}

	mIsSkeletal = true; // スケルトンを持っていることを明示

	// 3. アニメーターのリンク（Animatorが別コンポーネントの場合）
	//    ActorObjectにアタッチされているAnimatorコンポーネントを取得し、mAnimatorに設定します。
	//    (注: Animatorクラスがある前提)
	//    Animator* animator = mOwner->GetComponent<Animator>(); 
	//    SetAnimator(animator);
}
