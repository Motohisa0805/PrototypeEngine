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
				if (mAnimator)
				{
					// Set the matrix palette
					shader->SetMatrixUniforms("uMatrixPalette",
						&mAnimator->GetPalette().mEntry[0],
						SkeletonLayout::MAX_SKELETON_BONES);
				}
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
				if (mAnimator)
				{
					// Set the matrix palette
					shader->SetMatrixUniforms("uMatrixPalette",
						&mAnimator->GetPalette().mEntry[0],
						SkeletonLayout::MAX_SKELETON_BONES);
				}

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

void SkeletalMeshRenderer::SetSkeleton(Skeleton* sk, ActorObject* actor)
{
	mSkeleton = nullptr;
	mSkeleton = sk;


	if (!mSkeleton) { return; }

	const auto& bones = mSkeleton->GetBones();

	//1.mParentIndex を使って親子関係を構築
	for (size_t i = 0; i < bones.size(); i++)
	{
		const Skeleton::Bone& bone = bones[i];
		BoneActor* childActor = mSkeleton->GetBoneActor()[i];

		//mParentIndexは親ボーンのインデックス
		if (bone.mParent != -1)
		{
			// 親ボーンのActorを取得
			BoneActor* parentActor = mSkeleton->GetBoneActor()[bone.mParent];

			// Transform::AddChildActor() を呼び出して親子関係を結ぶ
			// Transform.cpp に実装されている AddChildActor を利用
			parentActor->AddChildActor(childActor);

			// (オプション) SkeletalMeshRendererのオーナーActorを親にする場合
			// childActor->SetParentActor(this->GetOwner());
		}
		else
		{
			//ルートボーンの場合
			//ルートアクターをSkeletonMeshRendererを持つオーナーのオブジェクトの子にする
			if (mOwner)
			{
				mOwner->AddChildActor(childActor);
			}
		}
		/*
		// ローカルバインドポーズを設定
		// BoneActorのTransformにボーンのローカル位置と回転を設定し、
		// ボーンオフセット（BindPose）を反映させます。
		Matrix4 bindPose = bone.mLocalBindPose;
		childActor->SetLocalPosition(bindPose.GetTranslation());
		childActor->SetLocalRotation(bindPose.GetRotation());
		childActor->SetLocalScale(bindPose.GetScale());
		*/
	}
}

void SkeletalMeshRenderer::SetAnimator(Animator* animator)
{
	mAnimator = nullptr;
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
	/*
	if (mSkeleton != nullptr)
	{
		mSkeleton->SetParentActor(mOwner); // Actorにスケルトンを設定
	}
	*/

	mIsSkeletal = true; // スケルトンを持っていることを明示

	// 3. アニメーターのリンク（Animatorが別コンポーネントの場合）
	//    ActorObjectにアタッチされているAnimatorコンポーネントを取得し、mAnimatorに設定します。
	//    (注: Animatorクラスがある前提)
	//    Animator* animator = mOwner->GetComponent<Animator>(); 
	//    SetAnimator(animator);
}

void SkeletalMeshRenderer::DrawGUI()
{
	//MeshRendererのプロパティ
	ImGui::Text("SkeletalMeshRenderer");

	//1.ファイルパスの取得
	string currentPath = mFilePath;
	static char pathBuffer[256];
	strncpy_s(pathBuffer, currentPath.c_str(), sizeof(pathBuffer));
	pathBuffer[sizeof(pathBuffer) - 1] = '\0';

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
			mFilePath = path;

			Skeleton* sk = mGame->GetSkeleton(path);
			mSkeleton = sk;
			//ボーンの親子関係を構築
			if (mSkeleton != nullptr)
			{
				SetSkeleton(mSkeleton, mOwner);
			}
			mIsSkeletal = true;
		}
		ImGui::EndDragDropTarget();
	}
	/*
	// ボタンクリックでファイル選択ダイアログを開く実装
	if (ImGui::Button("Load Mesh from File"))
	{
		// 外部のファイル選択ダイアログ (例: nativefiledialog) を開き、
		// 選択されたファイルパスを meshRenderer->Load(...) に渡す。
	}
	*/

	ImGui::Separator();
}
