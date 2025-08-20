#include "Renderer.h"
#include "BaseScene.h"
#include <GL/glew.h>
#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"
#include "VertexArray.h"
#include "MeshRenderer.h"
#include "ParticleSystem.h"
#include "Canvas.h"
#include "Image.h"
#include "SkeletalMeshRenderer.h"
#include "GBuffer.h"
#include "ShadowMap.h"
#include "PointLightComponent.h"
#include "DebugGrid.h"
#include "DirectionalLightComponent.h"
#include "SkyBoxRenderer.h"

#include "SceneViewEditor.h"

Renderer::Renderer()
	: mNowScene(nullptr)
	, mSpriteShader(nullptr)
	, mMeshShader(nullptr)
	, mSkinnedShader(nullptr)
	, mGBuffer(nullptr)
	, mGGlobalShader(nullptr)
	, mShadowMap(nullptr)
	, mShadowShader(nullptr)
	, mSkinnedShadowShader(nullptr)
	, mGPointLightShader(nullptr)
	, mContext(nullptr)
	, mFanSpriteVerts(nullptr)
	, mPointLightMesh(nullptr)
	, mSpriteVerts(nullptr)
	, mWindow(nullptr)
	, mArrowShader(nullptr)
	, mAxisVAO(nullptr)
	, mDebugGrid(nullptr)
	, mGridShader(nullptr)
	, mParticleShader(nullptr)
	, mSkyBoxRenderer(nullptr)
	, mSkyBoxShader(nullptr)
{
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize(float screenWidth, float screenHeight)
{
	// OpenGLの属性を設定する
	// コアOpenGLプロファイルを使用
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// バージョン3.3を指定
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// RGBAチャネルごとに8ビットのカラーバッファをリクエスト
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	// ダブルバッファリングを有効にする
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// OpenGLにハードウェアアクセラレーションを使用
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	//SDL_Windowを作成する
	mWindow = SDL_CreateWindow("SDL&OpenGLProject",static_cast<int>(WindowRenderProperty::GetWidth()), static_cast<int>(WindowRenderProperty::GetHeight()), SDL_WINDOW_OPENGL);
	//エラーチェック
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	// OpenGLコンテキストを作成する
	mContext = SDL_GL_CreateContext(mWindow);
	// GLEWを初期化する
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("Failed to initialize GLEW.");
		return false;
	}
	// 一部のプラットフォームでは、GLEWが無害なエラーコードを出力するので、
	// これをクリア。
	glGetError();
	// 描画用の2D矩形を作成する
	CreateSpriteVerts();
	//オブジェクトの方向矢印用の頂点配列を作成
	CreateAxisVerts();

	//スカイボックスを生成
	mSkyBoxRenderer = new SkyBoxRenderer();

	//デバッググリッド生成
	mDebugGrid = new DebugGrid();
	// Gバッファを作成する
	mGBuffer = new GBuffer();
	int width = static_cast<int>(WindowRenderProperty::GetWidth());
	int height = static_cast<int>(WindowRenderProperty::GetHeight());
	if (!mGBuffer->Create(width, height))
	{
		SDL_Log("Failed to create G-buffer.");
		return false;
	}
	//シャドウマップを作成する
	mShadowMap = new ShadowMap();
	if(!mShadowMap->Initialize(width, height))
	{
		SDL_Log("Failed to create shadow map.");
		return false;
	}
	// シェーダーを作成/コンパイルできることを確認してください
	if (!LoadShaders())
	{
		SDL_Log("Failed to load shaders.");
		return false;
	}

	mSceneViewEditor = new SceneViewEditor();
	mSceneViewEditor->CreateSceneFBO(width, height);
	
	GUIWinMain::SetRenderer(this);
	return true;
}

bool Renderer::LoadShaders()
{
	mSkyBoxShader = new Shader();
	if (!mSkyBoxShader->Load("Skybox.vert", "Skybox.frag"))
	{
		return false;
	}
	mSkyBoxShader->SetActive();
	mSkyBoxShader->SetIntUniform("skybox", 0);

	// スプライトシェーダーを作成する
	mSpriteShader = new Shader();
	if (!mSpriteShader->Load("Sprite.vert", "Sprite.frag"))
	{
		return false;
	}
	mSpriteShader->SetActive();
	Matrix4 spriteViewProj = Matrix4::CreateSimpleViewProj(WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight());
	// ビュー投影行列を設定する
	mSpriteShader->SetMatrixUniform("uViewProj", spriteViewProj);

	// ビュー投影行列を設定する
	mView = Matrix4::CreateLookAt(WindowRenderProperty::GetViewEye(), WindowRenderProperty::GetViewTarget(), WindowRenderProperty::GetViewUp());
	mProjection = Matrix4::CreatePerspectiveFOV(Math::ToRadians(WindowRenderProperty::GetFieldOfView()), WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight(), WindowRenderProperty::GetCameraNear(), WindowRenderProperty::GetCameraFar());

	// 基本的なメッシュシェーダーを作成する
	mMeshShader = new Shader();
	if (!mMeshShader->Load("Phong.vert", "GBufferWrite.frag"))
	{
		return false;
	}
	mMeshShader->SetActive();
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);

	// スキンシェーダーを作成する
	mSkinnedShader = new Shader();
	if (!mSkinnedShader->Load("Skinned.vert", "GBufferWrite.frag"))
	{
		return false;
	}
	mSkinnedShader->SetActive();
	mSkinnedShader->SetMatrixUniform("uViewProj", mView * mProjection);

	mArrowShader = new Shader();
	if (!mArrowShader->Load("Arrow.vert", "Arrow.frag"))
	{
		return false;
	}

	// パーティクルシェーダーを作成する
	mParticleShader = new Shader();
	if (!mParticleShader->Load("Sprite.vert", "ParticleSprite.frag"))
	{
		return false;
	}
	mParticleShader->SetActive();
	mParticleShader->SetMatrixUniform("uViewProj", mView * mProjection);

	// GBufferから描画するためのシェーダーを作成する（グローバルライティング）
	mGGlobalShader = new Shader();
	if (!mGGlobalShader->Load("GBufferGlobal.vert", "GBufferGlobal.frag"))
	{
		return false;
	}
	// GBufferのために、各サンプラーをインデックスに関連付ける
	mGGlobalShader->SetActive();
	mGGlobalShader->SetIntUniform("uGDiffuse", 0);
	mGGlobalShader->SetIntUniform("uGNormal", 1);
	mGGlobalShader->SetIntUniform("uGWorldPos", 2);
	// ビュー投影はただのスプライトのものです
	mGGlobalShader->SetMatrixUniform("uViewProj", spriteViewProj);
	// 世界の変形スケールが画面に適用され、yが反転します
	Matrix4 gbufferWorld = Matrix4::CreateScale(WindowRenderProperty::GetWidth(), -WindowRenderProperty::GetHeight(),
		1.0f);
	mGGlobalShader->SetMatrixUniform("uWorldTransform", gbufferWorld);
	glActiveTexture(GL_TEXTURE3); // 空いているテクスチャユニットを選択
	glBindTexture(GL_TEXTURE_2D, mShadowMap->GetDepthTexture());
	mGGlobalShader->SetIntUniform("uShadowMap", 3);

	mShadowShader = new Shader();
	if (!mShadowShader->Load("ShadowDepth.vert", "ShadowDepth.frag"))
	{
		return false;
	}
	mSkinnedShadowShader = new Shader();
	if (!mSkinnedShadowShader->Load("SkinnedShadowDepth.vert", "SkinnedShadowDepth.frag"))
	{
		return false;
	}

	// GBufferからポイントライト用のシェーダーを作成する
	mGPointLightShader = new Shader();
	if (!mGPointLightShader->Load("BasicMesh.vert","GBufferPointLight.frag"))
	{
		return false;
	}
	// サンプラーインデックスを設定する
	mGPointLightShader->SetActive();
	mGPointLightShader->SetIntUniform("uGDiffuse", 0);
	mGPointLightShader->SetIntUniform("uGNormal", 1);
	mGPointLightShader->SetIntUniform("uGWorldPos", 2);
	mGPointLightShader->SetVector2Uniform("uScreenDimensions",Vector2(WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight()));
	//グリッドを描画するためのシェーダーを作成する
	mGridShader = new Shader();
	if (!mGridShader->Load("Grid.vert", "Grid.frag"))
	{
		return false;
	}
	return true;
}

void Renderer::MeshOrderUpdate()
{
	// 1. 一時リストに分離
	std::vector<MeshRenderer*> opaqueList;
	std::vector<MeshRenderer*> transparentList;
	// 透明オブジェクトと不透明オブジェクトを分ける
	for (auto& mesh : mMeshComps)
	{
		if (!mesh->GetVisible()) continue;
		//MeshRenderer内のMeshを1つずつチェック
		for (auto& m : mesh->GetMeshs())
		{
			// nullptrチェック
			if (!m) continue; 
			// マテリアルがない場合はスキップ
			if (m->GetMaterialInfo().empty()) continue; 
			const auto& materials = m->GetMaterialInfo();
			bool isTransparent = false;
			for (const auto& mat : materials)
			{
				// 不透明度が1未満なら透明とみなす
				if (mat.Color.w < 1.0f) 
				{
					isTransparent = true;
					break;
				}
			}

			if (isTransparent)
				transparentList.push_back(mesh);
			else
				opaqueList.push_back(mesh);
		}
	}

	// 2. 透明オブジェクトはカメラからの距離でソート（遠い順）
	Matrix4 view = mView;
	view.Invert();
	// ビュー行列の逆行列からカメラ位置取得
	Vector3 cameraPos = view.GetTranslation(); 
	std::sort(transparentList.begin(), transparentList.end(),
		[&](MeshRenderer* a, MeshRenderer* b)
		{
			float distA = (a->GetOwner()->GetPosition() - cameraPos).LengthSq();
			float distB = (b->GetOwner()->GetPosition() - cameraPos).LengthSq();
			return distA > distB; // 遠い順に
		}
	);
	// 3. mMeshComps を再構築
	mMeshComps.clear();
	mMeshComps.insert(mMeshComps.end(), opaqueList.begin(), opaqueList.end());
	mMeshComps.insert(mMeshComps.end(), transparentList.begin(), transparentList.end());
}

void Renderer::Draw()
{
	//Meshの順番を変更
	MeshOrderUpdate();

	// ライト視点で深度情報をシャドウマップに描画
	DrawShadow3DScene();

	// G-bufferに3Dシーンを描画します。
	Draw3DScene(mGBuffer->GetBufferID(), mView, mProjection, 1.0f, true);

	Draw3DScene(mSceneViewEditor->GetSceneFBO(), mView, mProjection, 1.0f, true);
	// フレームバッファをゼロ（スクリーンのフレームバッファ）に戻します
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Gバッファから描画する
	DrawFromGBuffer();

	// すべてのスプライトコンポーネントを描画する
	// 深度バッファリングを無効にする
	glDisable(GL_DEPTH_TEST);
	// カラー バッファでアルファ ブレンディングを有効にします
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// シェーダー/VAOをアクティブに設定
	mSpriteShader->SetActive();

	mSpriteVerts->SetActive();

	for (auto ui : mNowScene->GetImageStack())
	{
		if (ui->GetState() == Image::EActive) 
		{
			if (ui->GetFillMethod() == Image::Radial360)
			{
				int count = CreateFanSpriteVerts(ui->GetFillAmount(),30);
				ui->SetVerticesCount(count);
				mFanSpriteVerts->SetActive();
			}
			else
			{
				mSpriteVerts->SetActive();
			}
			ui->Draw(mSpriteShader);
		}
	}

	if (GameStateClass::mDebugFrag)
	{
		for (auto ui : mNowScene->GetDebugImageStack())
		{
			if (ui->GetState() == Image::EActive)
			{
				if (ui->GetFillMethod() == Image::Radial360)
				{
					int count = CreateFanSpriteVerts(ui->GetFillAmount(), 30);
					ui->SetVerticesCount(count);
					mFanSpriteVerts->SetActive();
				}
				else
				{
					mSpriteVerts->SetActive();
				}
				ui->Draw(mSpriteShader);
			}
		}
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// バッファを入れ替える
	SDL_GL_SwapWindow(mWindow);
}

void Renderer::Draw3DScene(unsigned int framebuffer, const Matrix4& view, const Matrix4& proj,float viewPortScale, bool lit)
{
	// 現在のフレームバッファを設定する
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// スケールに基づいてビューポートサイズを設定します
	glViewport(0, 0,static_cast<int>(WindowRenderProperty::GetWidth() * viewPortScale),static_cast<int>(WindowRenderProperty::GetHeight() * viewPortScale));
	
	// カラー バッファ/深度バッファをクリア
	glClearColor(Color::mClearColor.x, Color::mClearColor.y, Color::mClearColor.z, Color::mClearColor.w);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// スカイボックス描画
	mSkyBoxRenderer->Draw(mSkyBoxShader, view, proj);

	// メッシュコンポーネントを描画する深度バッファリングを有効にする
	// アルファブレンドを無効にする
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// メッシュ（静的）
	mMeshShader->SetActive();
	mMeshShader->SetMatrixUniform("uViewProj", view * proj);
	SetLightUniforms(mMeshShader, view);

	for (auto mc : mMeshComps)
	{
		if (mc->GetVisible())
		{
			mc->Draw(mMeshShader);
		}
	}

	// スキンメッシュを有効
	mSkinnedShader->SetActive();
	// ビュー投影行列を更新する
	mSkinnedShader->SetMatrixUniform("uViewProj", view * proj);
	// 照明のユニフォームを更新する
	SetLightUniforms(mSkinnedShader, view);
	for (auto sk : mSkeletalMeshes)
	{
		if (sk->GetVisible())
		{
			sk->Draw(mSkinnedShader);
		}
	}
	// 2. パーティクルなど半透明物体を描画
	//  Z比較を有効（必須）
	glEnable(GL_DEPTH_TEST);                     
	//  Zバッファ書き込みを防ぐ
	glDepthMask(GL_FALSE);                       
	//  透過合成
	glEnable(GL_BLEND);                          
	// アルファブレンド
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	//パーティクルシステムの描画
	mParticleShader->SetActive();
	//パーティクルで使うため板ポリをアクティブに設定
	mSpriteVerts->SetActive(); // 板ポリ
	mParticleShader->SetMatrixUniform("uViewProj", view * proj);
	for(auto p : mParticlesComps)
	{
		if (p->IsVisible())
		{
			p->Draw(mParticleShader);
		}
	}
	//デバッグ描画
	if (GameStateClass::mDebugFrag)
	{
		mArrowShader->SetActive();
		mArrowShader->SetMatrixUniform("uViewProj", view * proj);
		for(auto actor : mNowScene->GetActors())
		{
			if (actor->GetState() == ActorObject::EActive)
			{
				// アクターのデバッグ描画
				mArrowShader->SetMatrixUniform("uModel", actor->GetWorldTransform());

				mAxisVAO->SetActive(); // 6頂点（3軸 × 2点）
				glLineWidth(3.0f); // 線の太さを3ピクセルに設定
				glDrawArrays(GL_LINES, 0, 6);
			}
		}
		mDebugGrid->Draw(mGridShader, view * proj);
	}

	glDepthMask(GL_TRUE);  // 書き込みを戻す
}

void Renderer::DrawShadow3DScene()
{
	mShadowMap->UpdateLightMatrix(mDirLight.mDirection.Normalized(), Vector3::Zero);
	Matrix4 lightViewProj = mShadowMap->GetLightViewProj();
	mGGlobalShader->SetActive();
	mGGlobalShader->SetMatrixUniform("uLightViewProj", lightViewProj);

	mShadowMap->BeginRender();

	glClearColor(Color::mClearColor.x, Color::mClearColor.y, Color::mClearColor.z, Color::mClearColor.w);
	glDepthMask(GL_TRUE);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);


	mShadowShader->SetActive();
	mShadowShader->SetMatrixUniform("uLightViewProj", lightViewProj);
	for (auto mc : mMeshComps)
	{
		if (mc->GetVisible())
		{
			mc->DrawForShadowMap(mShadowShader);
		}
	}
	// スキンメッシュを有効
	mSkinnedShadowShader->SetActive();
	// ビュー投影行列を更新する
	mSkinnedShadowShader->SetMatrixUniform("uLightViewProj", lightViewProj);
	for (auto sk : mSkeletalMeshes)
	{
		if (sk->GetVisible())
		{
			sk->DrawForShadowMap(mSkinnedShadowShader);
		}
	}

	mShadowMap->EndRender(); 
}

void Renderer::DrawFromGBuffer()
{
	// グローバルライティングパスの深度テストを無効にします
	glDisable(GL_DEPTH_TEST);
	// グローバルGバッファシェーダをアクティブにする
	mGGlobalShader->SetActive();
	// スプライトの頂点クアッドを有効化する
	mSpriteVerts->SetActive();
	// Gバッファーテクスチャをサンプリングするように設定する
	mGBuffer->SetTexturesActive();

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mShadowMap->GetDepthTexture());
	mGGlobalShader->SetIntUniform("uShadowMap", 3);

	// 照明ユニフォームを設定する
	SetLightUniforms(mGGlobalShader, mView);
	// 三角形を描画
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	// Gバッファからデフォルトフレームバッファに深度バッファをコピーする
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer->GetBufferID());
	int width = static_cast<int>(WindowRenderProperty::GetWidth());
	int height = static_cast<int>(WindowRenderProperty::GetHeight());
	glBlitFramebuffer(0, 0, width, height,0, 0, width, height,GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	// 深度テストを有効にしますが、深度バッファへの書き込みを無効にします。
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	/*
	// 点光源シェーダーとメッシュをアクティブに設定します。
	mGPointLightShader->SetActive();
	size_t size = mPointLightMesh->GetVertexArrays().size();
	for (unsigned int i = 0; i < size; i++) 
	{
		mPointLightMesh->GetVertexArrays()[i]->SetActive();
	}
	*/
	// ビュー投影行列を設定する
	//mGPointLightShader->SetMatrixUniform("uViewProj",mView * mProjection);
	// サンプリングのためにGバッファーのテクスチャを設定します
	mGBuffer->SetTexturesActive();

	// 点光源の色は既存の色に追加される
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	/*
	// ポイントライトを描画
	for (PointLightComponent* p : mPointLights)
	{
		p->Draw(mGPointLightShader, mPointLightMesh);
	}
	*/
}

void Renderer::Shutdown()
{
	// Gバッファを取り除く
	if (mGBuffer)
	{
		mGBuffer->Destroy();
		delete mGBuffer;
		mGBuffer = nullptr;
	}
	// シャドウマップを取り除く
	if (mShadowMap)
	{
		delete mShadowMap;
		mShadowMap = nullptr;
	}

	if( mSceneViewEditor)
	{
		delete mSceneViewEditor;
		mSceneViewEditor = nullptr;
	}

	// ポイントライトを削除する
	while (!mPointLights.empty())
	{
		delete mPointLights.back();
		mPointLights.pop_back();
	}

	// 頂点配列
	if (mSpriteVerts)
	{
		delete mSpriteVerts;
		mSpriteVerts = nullptr;
	}
	// 2D画像用の頂点配列を解放
	if (mFanSpriteVerts)
	{
		delete mFanSpriteVerts;
		mFanSpriteVerts = nullptr;
	}
	// オブジェクトの方向矢印用の頂点配列を解放
	if (mAxisVAO)
	{
		delete mAxisVAO;
		mAxisVAO = nullptr;
	}
	// スカイボックスレンダラーを解放
	if( mSkyBoxRenderer)
	{
		delete mSkyBoxRenderer;
		mSkyBoxRenderer = nullptr;
	}
	// スプライトシェーダー
	if (mSpriteShader)
	{
		mSpriteShader->Unload();
		delete mSpriteShader;
		mSpriteShader = nullptr;
	}
	//メッシュシェーダーを解放
	if (mMeshShader)
	{
		mMeshShader->Unload();
		delete mMeshShader;
		mMeshShader = nullptr;
	}
	// スキンメッシュシェーダーを解放
	if (mSkinnedShader)
	{
		mSkinnedShader->Unload();
		delete mSkinnedShader;
		mSkinnedShader = nullptr;
	}
	// 矢印シェーダーを解放
	if (mArrowShader)
	{
		mArrowShader->Unload();
		delete mArrowShader;
		mArrowShader = nullptr;
	}
	// パーティクルシェーダーを解放
	if (mParticleShader)
	{
		mParticleShader->Unload();
		delete mParticleShader;
		mParticleShader = nullptr;
	}
	// Gバッファーのシェーダーを解放
	if (mGGlobalShader)
	{
		mGGlobalShader->Unload();
		delete mGGlobalShader;
		mGGlobalShader = nullptr;
	}
	// シャドウマップのシェーダーを解放
	if(mShadowShader)
	{
		mShadowShader->Unload();
		delete mShadowShader;
		mShadowShader = nullptr;
	}
	// スキンシャドウマップのシェーダーを解放
	if(mSkinnedShadowShader)
	{
		mSkinnedShadowShader->Unload();
		delete mSkinnedShadowShader;
		mSkinnedShadowShader = nullptr;
	}
	// Gバッファーのポイントライトシェーダーを解放
	if (mGPointLightShader)
	{
		mGPointLightShader->Unload();
		delete mGPointLightShader;
		mGPointLightShader = nullptr;
	}
	// グリッドシェーダーを解放
	if (mGridShader)
	{
		mGridShader->Unload();
		delete mGridShader;
		mGridShader = nullptr;
	}
	//スカイボックスを解放
	if( mSkyBoxShader)
	{
		mSkyBoxShader->Unload();
		delete mSkyBoxShader;
		mSkyBoxShader = nullptr;
	}
	//グリッドを解放
	if (mDebugGrid)
	{
		delete mDebugGrid;
		mDebugGrid = nullptr;
	}
	// OpenGLコンテキストとウィンドウ
	if (mContext)
	{
		SDL_GL_DestroyContext(mContext);
		mContext = nullptr;
	}
	if (mWindow)
	{
		SDL_DestroyWindow(mWindow);
		mWindow = nullptr;
	}
}

void Renderer::UnloadData()
{
	// テクスチャを破壊する
	for (auto i : mTextures)
	{
		i.second->Unload();
		delete i.second;
	}
	mTextures.clear();
	// メッシュを破壊する
	for (auto i : mMeshes)
	{
		i.second->Unload();
		delete i.second;
	}
	mMeshes.clear();
}


void Renderer::AddMeshComp(MeshRenderer* mesh)
{
	if (mesh->GetIsSkeletal())
	{
		SkeletalMeshRenderer* sk = static_cast<SkeletalMeshRenderer*>(mesh);
		mSkeletalMeshes.emplace_back(sk);
	}
	else
	{
		mMeshComps.emplace_back(mesh);
	}
}

void Renderer::RemoveMeshComp(MeshRenderer* mesh)
{
	if (mesh->GetIsSkeletal())
	{
		SkeletalMeshRenderer* sk = static_cast<SkeletalMeshRenderer*>(mesh);
		auto iter = std::find(mSkeletalMeshes.begin(), mSkeletalMeshes.end(), sk);
		mSkeletalMeshes.erase(iter);
	}
	else
	{
		auto iter = std::find(mMeshComps.begin(), mMeshComps.end(), mesh);
		mMeshComps.erase(iter);
	}
}

void Renderer::AddParticleComp(ParticleSystem* particle)
{
	mParticlesComps.emplace_back(particle);
}

void Renderer::RemoveParticleComp(ParticleSystem* particle)
{
	auto iter = std::find(mParticlesComps.begin(), mParticlesComps.end(), particle);
	mParticlesComps.erase(iter);
}

void Renderer::AddPointLight(PointLightComponent* light)
{
	mPointLights.emplace_back(light);
}

void Renderer::RemovePointLight(PointLightComponent* light)
{
	auto iter = std::find(mPointLights.begin(), mPointLights.end(), light);
	mPointLights.erase(iter);
}

Texture* Renderer::GetTexture(const string& fileName)
{
	Texture* tex = nullptr;
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		tex = new Texture();
		if (tex->Load(fileName))
		{
			mTextures.emplace(fileName, tex);
		}
		else
		{
			delete tex;
			tex = nullptr;
		}
	}
	return tex;
}

Mesh* Renderer::GetMesh(const string& fileName)
{
	string file = File_P::ModelPath + fileName;
	Mesh* m = nullptr;
	auto iter = mMeshes.find(file);
	if (iter != mMeshes.end())
	{
		m = iter->second;
	}
	else
	{
		m = new Mesh();
		if (m->LoadFromMeshBin(file, this))
		{
			mMeshes.emplace(file, m);
		}
		else if (m->Load(file, this))
		{
			mMeshes.emplace(file, m);
		}
		else
		{
			delete m;
			m = nullptr;
		}
	}
	return m;
}

vector<class Mesh*> Renderer::GetMeshs(const string& fileName)
{
	//ファイルパス追加
	string filePath = File_P::ModelPath + fileName;
	//返す複数のメッシュ
	vector<class Mesh*> ms;
	//メッシュの数を確認する処理
	Mesh* m = nullptr;
	m = new Mesh();
	int maxMesh = m->CheckMeshIndex(filePath, this);
	if (m)
	{
		//メッシュの解放
		m->Unload();
		delete m;
	}
	for (int i = 0; i < maxMesh; i++)
	{
		string inTex = std::to_string(i);
		Mesh* mesh = nullptr;
		auto iter = mMeshes.find(filePath + inTex.c_str());
		//すでに読み込んでいるものならそこから取得
		if (iter != mMeshes.end())
		{
			mesh = iter->second;
		}
		else
		{
			mesh = new Mesh();
			//ここにLoad前にバイナリファイルがあるかを確認する
			if (mesh->LoadFromMeshBin(filePath, this, i))
			{
				mMeshes.emplace(filePath + inTex.c_str(), mesh);
			}
			else if (mesh->Load(filePath, this , i))
			{
				mMeshes.emplace(filePath + inTex.c_str(), mesh);
			}
			else
			{
				delete mesh;
				mesh = nullptr;
			}
		}

		if (mesh != nullptr) {
			ms.push_back(mesh);
		}
	}
	return ms;
}

void Renderer::CreateSpriteVerts()
{
	float vertices[] = {
		-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
		0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
		0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
		-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	mSpriteVerts = new VertexArray(vertices, 4, VertexArray::PosNormTex, indices, 6);
}
int Renderer::CreateFanSpriteVerts(float fillRatio, int maxSegments)
{
	// 作成した頂点配列とインデックス配列でVertexArray作成

	if (mFanSpriteVerts)
		delete mFanSpriteVerts;
	mFanSpriteVerts = new VertexArray(fillRatio, maxSegments);
	return mFanSpriteVerts->GetNumVerts();
}

void Renderer::CreateAxisVerts()
{
	std::vector<AxisVertex> axisVerts = {
		// X軸（赤）
		{ Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f) },
		{ Vector3(1.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f) },

		// Y軸（緑）
		{ Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f) },
		{ Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f) },

		// Z軸（青）
		{ Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f) },
		{ Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f) },
	};
	mAxisVAO = new VertexArray(axisVerts);
}

void Renderer::SetLightUniforms(Shader* shader, const Matrix4& view)
{
	// カメラの位置は逆さまの視点からです
	Matrix4 invView = view;
	invView.Invert();
	shader->SetVectorUniform("uCameraPos", invView.GetTranslation());
	// Ambient light
	shader->SetVectorUniform("uAmbientLight", mDirLight.mAmbientColor);
	// Directional light
	shader->SetVectorUniform("uDirLight.mDirection",mDirLight.mDirection);
	shader->SetVectorUniform("uDirLight.mDiffuseColor",mDirLight.mDiffuseColor);
	shader->SetVectorUniform("uDirLight.mSpecColor",mDirLight.mSpecColor);
}

Vector3 Renderer::Unproject(const Vector3& screenPoint) const
{
	// screenPointをデバイス座標（-1から+1の間）に変換する
	Vector3 deviceCoord = screenPoint;
	deviceCoord.x /= (WindowRenderProperty::GetWidth()) * 0.5f;
	deviceCoord.y /= (WindowRenderProperty::GetHeight()) * 0.5f;

	// 反投影行列でベクトルを変換する
	Matrix4 unprojection = mView * mProjection;
	unprojection.Invert();
	return Vector3::TransformWithPerspDiv(deviceCoord, unprojection);
}

void Renderer::GetScreenDirection(Vector3& outStart, Vector3& outDir) const
{
	// スタートポイントを取得する（近くの平面の画面の中心）
	Vector3 screenPoint(0.0f, 0.0f, 0.0f);
	outStart = Unproject(screenPoint);
	// 画面の中央、近くと遠くの間にエンドポイントを取得します。
	screenPoint.z = 0.9f;
	Vector3 end = Unproject(screenPoint);
	// 方向ベクトルを取得する
	outDir = end - outStart;
	outDir.Normalize();
}
