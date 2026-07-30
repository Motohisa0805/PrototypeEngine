#include "Renderer.h"
#include "AudioSystem.h"
#include "BaseCamera.h"
#include "Canvas.h"
#include "DebugGrid.h"
#include "DirectionalLightComponent.h"
#include "EditorSettingsManager.h"
#include "EngineWindow.h"
#include "GBuffer.h"
#include "GameWinMain.h"
#include "HierarchyPanel.h"
#include "Image.h"
#include "LocalLight.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "ParticleSystem.h"
#include "SceneEditorCamera.h"
#include "SceneViewEditor.h"
#include "SceneViewPanel.h"
#include "ShadowMap.h"
#include "SkeletalMeshRenderer.h"
#include <GL/glew.h>

Renderer::Renderer()
    : mWindowTitle("PrototypeEngine - Windows - Ver0.01 <OpenGL 2.2.0,SDL3>")
    , mRunScene(nullptr)
    , mSpriteShader(nullptr)
    , mMeshShader(nullptr)
    , mSkinnedShader(nullptr)
    , mGBuffer(nullptr)
    , mGGlobalShader(nullptr)
    , mShadowMap(nullptr)
    , mShadowShader(nullptr)
    , mSkinnedShadowShader(nullptr)
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
    , mDrawCalls(0)
    , mGameSceneViewEditor(nullptr)
{
}

Renderer::~Renderer() {}

bool Renderer::Initialize(float screenWidth, float screenHeight)
{
    // OpenGLの属性を設定する
    // コアOpenGLプロファイルを使用
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    // バージョン3.3を指定
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    // RGBAチャネルごとに8ビットのカラーバッファをリクエスト
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    // Zファイティング防止のため数値は高めに深度バッファをリクエスト
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    // ダブルバッファリングを有効にする
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // OpenGLにハードウェアアクセラレーションを使用
    // 動作軽減のため描画用GPUを使用するように要求
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    // SDL_Windowを作成する
    mWindow = SDL_CreateWindow(
        mWindowTitle.c_str(),
        static_cast<int>(WindowRenderProperty::GetWidth()),
        static_cast<int>(WindowRenderProperty::GetHeight()),
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
    // エラーチェック
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }
    // OpenGLコンテキストを作成する
    mContext = SDL_GL_CreateContext(mWindow);
    if (!mContext)
    {
        // コンテキスト作成失敗（グラボのドライバが古い、設定が無茶すぎる等）
        SDL_Log("Failed to create OpenGL context: %s", SDL_GetError());
        return false;
    }
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
    // オブジェクトの方向矢印用の頂点配列を作成
    CreateAxisVerts();

    // スカイボックスを生成
    mSkyBoxRenderer = new SkyBoxRenderer();

    // デバッググリッド生成
    mDebugGrid = new DebugGrid();
    // Gバッファを作成する
    mGBuffer   = new GBuffer();
    int width  = static_cast<int>(WindowRenderProperty::GetWidth());
    int height = static_cast<int>(WindowRenderProperty::GetHeight());
    if (!mGBuffer->Create(width, height))
    {
        SDL_Log("Failed to create G-buffer.");
        return false;
    }

    // シャドウマップを作成する
    mShadowMap = new ShadowMap();
    if (!mShadowMap->Initialize(width, height))
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

    Texture::InitializeDefaultTextures();

    // ゲームシーンのFBOを作成
    mGameSceneViewEditor = new SceneViewEditor();
    mGameSceneViewEditor->CreateSceneFBO(width, height);

    GUIEditorManager::SetRenderer(this);
    return true;
}

void Renderer::BuildStaticBatch()
{
    // 1. 古いバッチの破棄とクリア
    for (auto& pair : mAntiTransparentBatchesMap)
    {
        if (pair.second.gBatchVertexArray)
        {
            delete pair.second.gBatchVertexArray;
        }
    }
    for (auto& pair : mTransparentBatchesMap)
    {
        if (pair.second.gBatchVertexArray)
        {
            delete pair.second.gBatchVertexArray;
        }
    }

    // mStaticMeshBatches.clear();
    mAntiTransparentBatchesMap.clear();
    mTransparentBatchesMap.clear();
    // 2. メッシュごとの処理
    for (auto mc : mMeshCompArray)
    {
        if (mc->GetOwner()->GetStatic() !=
            ActorInformation::StaticTag::Occluder_Static)
            continue;
        Matrix4 world = mc->GetActor()->GetTransform()->GetLocalTransform();
        for (auto mesh : mc->GetMeshs())
        {
            for (int i = 0; i < mesh->GetVertexArrays().size(); i++)
            {
                if (mesh->GetMaterialInfo()[i].Color.w < 1.0f)
                {
                    // 半透明バッチに追加
                    BuildMeshBatch(
                        mesh, world,
                        mTransparentBatchesMap[&mesh->GetMaterialInfo()[i]], i);
                }
                else
                {
                    // アンチ半透明バッチに追加
                    BuildMeshBatch(
                        mesh, world,
                        mAntiTransparentBatchesMap[&mesh->GetMaterialInfo()[i]],
                        i);
                }
            }
        }
    }

    // 3. 全てのバッチのVertexArrayを生成
    for (auto& pair : mAntiTransparentBatchesMap)
    {
        StaticMeshBatch& batch = pair.second;
        if (!batch.gAllVertices.empty())
        {
            batch.gBatchVertexArray = new VertexArray(
                batch.gAllVertices.data(), batch.gAllVertices.size(),
                VertexArray::PosNormTex, batch.gAllIndices.data(),
                batch.gAllIndices.size());
        }
    }

    for (auto& pair : mTransparentBatchesMap)
    {
        StaticMeshBatch& batch = pair.second;
        if (!batch.gAllVertices.empty())
        {
            batch.gBatchVertexArray = new VertexArray(
                batch.gAllVertices.data(), batch.gAllVertices.size(),
                VertexArray::PosNormTex, batch.gAllIndices.data(),
                batch.gAllIndices.size());
        }
    }
}

void Renderer::BuildMeshBatch(Mesh* mesh, Matrix4 world,
                              StaticMeshBatch& outBatch, int index)
{
    outBatch.gBatchTexture  = mesh->GetTexture(index);
    outBatch.gBatchMaterial = mesh->GetMaterialInfo()[index];

    const std::vector<Vertex>&   meshVertices = mesh->GetVertices();
    const std::vector<uint32_t>& meshIndices  = mesh->GetIndices();

    // 現在のバッチの頂点数をオフセットにする
    unsigned int vertexOffset =
        static_cast<unsigned int>(outBatch.gAllVertices.size());
    for (const auto& v : meshVertices)
    {
        Vertex transformed = v;
        transformed.pos    = Vector3::Transform(v.pos, world);
        transformed.normal = Vector3::TransformNormal(v.normal, world);
        transformed.normal.Normalize();
        outBatch.gAllVertices.push_back(transformed);
    }

    // インデックスを結合（オフセットを考慮）
    for (uint32_t idx : meshIndices)
    {
        outBatch.gAllIndices.push_back(idx + vertexOffset);
    }
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
    Matrix4 spriteViewProj = Matrix4::CreateSimpleViewProj(
        WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight());
    // ビュー投影行列を設定する
    mSpriteShader->SetMatrixUniform("uViewProj", spriteViewProj);

    // ビュー投影行列を設定する
    mView       = Matrix4::CreateLookAt(WindowRenderProperty::GetViewEye(),
                                        WindowRenderProperty::GetViewTarget(),
                                        WindowRenderProperty::GetViewUp());
    mProjection = Matrix4::CreatePerspectiveFOV(
        Math::ToRadians(WindowRenderProperty::GetFieldOfView()),
        WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight(),
        WindowRenderProperty::GetCameraNear(),
        WindowRenderProperty::GetCameraFar());

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
    mGGlobalShader->SetIntUniform("uGPBR", 3);
    mGGlobalShader->SetIntUniform("uGEmissive", 4);
    mGGlobalShader->SetIntUniform("uSkybox", 5);
    mGGlobalShader->SetIntUniform("uShadowMap", 6);
    // ビュー投影はただのスプライトのものです
    mGGlobalShader->SetMatrixUniform("uViewProj", spriteViewProj);
    // 世界の変形スケールが画面に適用され、yが反転します
    Matrix4 gbufferWorld =
        Matrix4::CreateScale(WindowRenderProperty::GetWidth(),
                             -WindowRenderProperty::GetHeight(), 1.0f);
    mGGlobalShader->SetMatrixUniform("uWorldTransform", gbufferWorld);

    mShadowShader = new Shader();
    if (!mShadowShader->Load("ShadowDepth.vert", "ShadowDepth.frag"))
    {
        return false;
    }
    mSkinnedShadowShader = new Shader();
    if (!mSkinnedShadowShader->Load("SkinnedShadowDepth.vert",
                                    "SkinnedShadowDepth.frag"))
    {
        return false;
    }
    /*
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
    mGPointLightShader->SetVector2Uniform("uScreenDimensions",Vector2(WindowRenderProperty::GetWidth(),
    WindowRenderProperty::GetHeight()));
    */
    // グリッドを描画するためのシェーダーを作成する
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
    // not透明オブジェクトと不透明オブジェクトを分ける
    for (auto& mesh : mMeshCompArray)
    {
        if (!mesh->GetVisible())
            continue;

        bool isTransparent = false;
        for (auto& m : mesh->GetMeshs())
        {
            if (!m)
                continue;
            if (m->GetMaterialInfo().empty())
                continue;

            const auto& materials = m->GetMaterialInfo();
            for (const auto& mat : materials)
            {
                if (mat.Color.w < 1.0f)
                {
                    isTransparent = true;
                    break;
                }
            }
            if (isTransparent)
                break; // ← 一つでも透明なら終了
        }

        if (isTransparent)
            transparentList.push_back(mesh);
        else
            opaqueList.push_back(mesh);
    }

    // 2. 透明オブジェクトはカメラからの距離でソート（遠い順）
    Matrix4 view = mView;
    view.Invert();
    // ビュー行列の逆行列からカメラ位置取得
    Vector3 cameraPos = view.GetTranslation();
    std::sort(transparentList.begin(), transparentList.end(),
              [&](MeshRenderer* a, MeshRenderer* b)
              {
                  float distA =
                      (a->GetActor()->GetTransform()->GetPosition() - cameraPos)
                          .LengthSq();
                  float distB =
                      (b->GetActor()->GetTransform()->GetPosition() - cameraPos)
                          .LengthSq();
                  return distA > distB; // 遠い順に
              });
    // 3. mMeshComps を再構築
    mMeshCompArray.clear();
    mMeshCompArray.insert(mMeshCompArray.end(), opaqueList.begin(),
                          opaqueList.end());
    mMeshCompArray.insert(mMeshCompArray.end(), transparentList.begin(),
                          transparentList.end());
}

void Renderer::DrawWindowTitle()
{
    // ウィンドウの名前変更処理
    if (EditorSettingsManager::IsNoSaveFlag())
    {
        const string title = mWindowTitle + "No Save";
        if (SDL_GetWindowTitle(mWindow) != title)
        {
            SDL_SetWindowTitle(mWindow, title.c_str());
        }
    }
    else
    {
        SDL_SetWindowTitle(mWindow, mWindowTitle.c_str());
    }
}

void Renderer::StartDraw()
{
    // ウィンドウのタイトル描画
    DrawWindowTitle();
    // 複数のカメラからメインカメラからmViewを設定
    for (auto cam : mRunScene->GetCameras())
    {
        if (cam.second->IsMain())
        {
            // ビュー行列をレンダラーとオーディオシステムに渡す
            mView = cam.second->GetViewMatrix();
            mRunScene->GetAudioSystem()->SetListener(mView);
            break;
        }
    }
    // Meshの順番を変更
    MeshOrderUpdate();
    // ライト視点で深度情報をシャドウマップに描画
    DrawShadow3DScene();

    //***SceneViewEditorのSceneFBOに描画
    // G-bufferに3Dシーンを描画します。
    for (SceneViewPanel* scene : GUIEditorManager::GetSceneViewPanels())
    {
        EditorDraw3DScene(scene, scene->GetSceneBuffer()->GetBufferID(),
                          scene->GetSceneEditorCamera()->GetViewMatrix(),
                          mProjection, 1.0f, true);
        // Gバッファから描画する
        DrawFromGBufferForEditor(scene);
    }

    // 描画呼び出しのカウンターをリセット
    mDrawCalls = 0;
    //***gameViewEditorのGameSceneFBOに描画***
    // G-bufferに3Dシーンを描画します。
    Draw3DScene(mGBuffer->GetBufferID(), mView, mProjection, 1.0f, true);
    // Gバッファから描画する
    DrawFromGBuffer();
    // すべてのスプライトコンポーネントを描画する
    // 深度バッファリングを無効にする
    glDisable(GL_DEPTH_TEST);
    // カラー バッファでアルファ ブレンディングを有効にします
    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    //===2D描画のためのFBOを開始===

    // 2D画像を描画するためのブレンド関数を設定
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // シェーダー/VAOをアクティブに設定
    mSpriteShader->SetActive();

    mSpriteVerts->SetActive();

    // スクリーン座標系 (0,0) が左上、(ウィンドウサイズX,ウィンドウサイズY)
    // が右下になるようにしたい場合
    Matrix4 ortho = Matrix4::CreateOrtho(
        (float)WindowRenderProperty::GetWidth(),
        (float)WindowRenderProperty::GetHeight(), -1.0f, 1.0f);
    mSpriteShader->SetMatrixUniform("uViewProj", ortho);

    // UI単体で描画処理
    for (auto ui : mImageCompArray)
    {
        if (ui->GetOwner()->GetState() == Entity::EActive &&
            ui->GetUIActor()->GetRectTransform()->GetParentActor() == nullptr)
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
    // Canvasを通して描画(Unityの設計を元)
    for (auto canvas : mCanvasActorArray)
    {
        for (auto ui : canvas->GetRectTransform()->GetChildActorList())
        {
            if (auto image = ui->GetComponent<Image>())
            {
                if (ui->GetState() == Entity::EActive)
                {
                    if (image->GetFillMethod() == Image::Radial360)
                    {
                        int count =
                            CreateFanSpriteVerts(image->GetFillAmount(), 30);
                        image->SetVerticesCount(count);
                        mFanSpriteVerts->SetActive();
                    }
                    else
                    {
                        mSpriteVerts->SetActive();
                    }
                    image->Draw(mSpriteShader);
                }
            }
        }
    }
    // FBO終了
    // フレームバッファをゼロ（スクリーンのフレームバッファ）に戻します
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::EndDraw()
{
    // デフォルトフレームバッファをクリア
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, (int)WindowRenderProperty::GetWidth(),
               (int)WindowRenderProperty::GetHeight());
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // バッファを入れ替える
    SDL_GL_SwapWindow(mWindow);
}

void Renderer::EditorDraw3DScene(SceneViewPanel* scene,
                                 unsigned int framebuffer, const Matrix4& view,
                                 const Matrix4& proj, float viewPortScale,
                                 bool lit)
{
    // 現在のフレームバッファを設定する
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // スケールに基づいてビューポートサイズを設定します
    Vector2 sceneWinSize = scene->GetSceneWinSize();
    glViewport(0, 0, (int)sceneWinSize.x * viewPortScale,
               (int)sceneWinSize.y * viewPortScale);

    // カラー バッファ/深度バッファをクリア
    glClearColor(Color::mClearColor.x, Color::mClearColor.y,
                 Color::mClearColor.z, Color::mClearColor.w);
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

    if (GUIEditorManager::IsPlaying())
    {
        // Staticバッチの描画 (1回のDrawCall)
        for (auto& pair : mAntiTransparentBatchesMap)
        {
            StaticMeshBatch& batch = pair.second;
            if (!batch.gBatchVertexArray)
                continue;
            mMeshShader->SetMatrixUniform("uWorldTransform", Matrix4::Identity);
            Texture* tex = batch.gBatchTexture;
            if (tex)
            {
                tex->SetActive();
            }
            else
            {
                mMeshShader->SetNoTexture();
            }
            MaterialInfo m = batch.gBatchMaterial;
            mMeshShader->SetColorUniform(m);
            batch.gBatchVertexArray->SetActive();
            glDrawElements(GL_TRIANGLES,
                           batch.gBatchVertexArray->GetNumIndices(),
                           GL_UNSIGNED_INT, nullptr);
        }
        for (auto& pair : mTransparentBatchesMap)
        {
            StaticMeshBatch& batch = pair.second;
            if (!batch.gBatchVertexArray)
                continue;
            mMeshShader->SetMatrixUniform("uWorldTransform", Matrix4::Identity);
            Texture* tex = batch.gBatchTexture;
            if (tex)
            {
                tex->SetActive();
            }
            else
            {
                mMeshShader->SetNoTexture();
            }
            MaterialInfo m = batch.gBatchMaterial;
            mMeshShader->SetColorUniform(m);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE); // 透明物体は深度書き込み無効（任意）
            batch.gBatchVertexArray->SetActive();
            glDrawElements(GL_TRIANGLES,
                           batch.gBatchVertexArray->GetNumIndices(),
                           GL_UNSIGNED_INT, nullptr);
        }
    }

    for (auto mc : mMeshCompArray)
    {
        // 静的オブジェクトは実行中のみ描画する
        if (GUIEditorManager::IsPlaying())
        {
            if (mc->GetVisible() &&
                mc->GetOwner()->GetStatic() !=
                    ActorInformation::StaticTag::Occluder_Static)
            {
                if (mc->Draw(mMeshShader))
                {
                    mDrawCalls++;
                }
            }
        }
        else
        {
            if (mc->GetVisible())
            {
                if (mc->Draw(mMeshShader))
                {
                    mDrawCalls++;
                }
            }
        }
    }

    // スキンメッシュを有効
    mSkinnedShader->SetActive();
    // ビュー投影行列を更新する
    mSkinnedShader->SetMatrixUniform("uViewProj", view * proj);
    // 照明のユニフォームを更新する
    SetLightUniforms(mSkinnedShader, view);
    for (auto sk : mSkeletalMeshArray)
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
    // パーティクルシステムの描画
    mParticleShader->SetActive();
    // パーティクルで使うため板ポリをアクティブに設定
    mSpriteVerts->SetActive(); // 板ポリ
    mParticleShader->SetMatrixUniform("uViewProj", view * proj);
    for (auto p : mParticlesCompArray)
    {
        if (p->IsVisible())
        {
            p->Draw(mParticleShader);
        }
    }
    for (SceneViewPanel* scene : GUIEditorManager::GetSceneViewPanels())
    {
        // デバッグ描画
        // オブジェクトの矢印描画
        mArrowShader->SetActive();
        mArrowShader->SetMatrixUniform("uViewProj", view * proj);
        Entity* actor = SelectionManager::GetSelectedActor();
        if (actor != nullptr && actor->GetState() == ActorObject::EActive)
        {
            // 1.カメラとオブジェクトの位置を取得
            Vector3 cameraPos =
                scene->GetSceneEditorCamera()->GetTransform()->GetPosition();
            Vector3 actorPos = actor->GetBaseTransform()->GetPosition();

            // 2.カメラとオブジェクトの距離を計算
            float distance = (actorPos - cameraPos).Length();

            // 3.画面サイズ固定のためのスケール定数を設定
            //  この値を調整することで、ギズモの見かけのサイズを変更できます。
            const float GIZMO_SCREEN_SIZE_FACTOR =
                0.15f; // 値が大きいほど画面上で大きく見える

            // 4. 距離に比例したワールドスケールを算出
            //  ギズモの長さを0.5mで定義しているため、それを基準に距離に応じてスケールを調整
            float baseLength =
                1.0f; // CreateAxisVertsで定義された各軸の長さ(1.0f)を基準とする
            float scale = distance * GIZMO_SCREEN_SIZE_FACTOR;

            // 5. 最小スケールを設定
            // (オブジェクトがカメラに近すぎる場合のサイズ制御)
            //  オブジェクトにめり込むほど近づいても、ギズモが小さくなりすぎないようにする
            if (scale < baseLength * 1.0f)
            {
                scale = baseLength * 1.0f;
            }

            // 6. 新しいモデル行列を作成
            // Scale -> Rotation -> Translation の順で適用
            Matrix4 gizmoModel = Matrix4::CreateScale(scale);
            gizmoModel *= Matrix4::CreateFromQuaternion(
                actor->GetBaseTransform()->GetRotation());
            gizmoModel *= Matrix4::CreateTranslation(actorPos);

            // オブジェクトのデバッグ描画
            mArrowShader->SetMatrixUniform("uModel", gizmoModel);
            // 6頂点（3軸 × 2点）
            mAxisVAO->SetActive();
            // 線の太さを3ピクセルに設定
            glLineWidth(6.0f);
            glDrawArrays(GL_LINES, 0, 6);
        }
        // デバッググリッド描画
        if (scene->IsDebugGridFrag())
        {
            if (mGridShader && scene->GetSceneEditorCamera())
            {
                mDebugGrid->Draw(mGridShader, view * proj,
                                 scene->GetSceneEditorCamera()
                                     ->GetTransform()
                                     ->GetPosition());
            }
        }
    }

    glDepthMask(GL_TRUE); // 書き込みを戻す
}

void Renderer::Draw3DScene(unsigned int framebuffer, const Matrix4& view,
                           const Matrix4& proj, float viewPortScale, bool lit)
{
    // 現在のフレームバッファを設定する
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // スケールに基づいてビューポートサイズを設定します
    glViewport(
        0, 0,
        static_cast<int>(WindowRenderProperty::GetWidth() * viewPortScale),
        static_cast<int>(WindowRenderProperty::GetHeight() * viewPortScale));

    // カラー バッファ/深度バッファをクリア
    glClearColor(Color::mClearColor.x, Color::mClearColor.y,
                 Color::mClearColor.z, Color::mClearColor.w);
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

    if (GUIEditorManager::IsPlaying())
    {
        for (auto& pair : mAntiTransparentBatchesMap)
        {
            StaticMeshBatch& batch = pair.second;
            if (!batch.gBatchVertexArray)
                continue;
            mMeshShader->SetMatrixUniform("uWorldTransform", Matrix4::Identity);
            Texture* tex = batch.gBatchTexture;
            if (tex)
            {
                tex->SetActive();
            }
            else
            {
                mMeshShader->SetNoTexture();
            }
            MaterialInfo m = batch.gBatchMaterial;
            mMeshShader->SetColorUniform(m);
            batch.gBatchVertexArray->SetActive();
            glDrawElements(GL_TRIANGLES,
                           batch.gBatchVertexArray->GetNumIndices(),
                           GL_UNSIGNED_INT, nullptr);
        }
        for (auto& pair : mTransparentBatchesMap)
        {
            StaticMeshBatch& batch = pair.second;
            if (!batch.gBatchVertexArray)
                continue;
            mMeshShader->SetMatrixUniform("uWorldTransform", Matrix4::Identity);
            Texture* tex = batch.gBatchTexture;
            if (tex)
            {
                tex->SetActive();
            }
            else
            {
                mMeshShader->SetNoTexture();
            }
            MaterialInfo m = batch.gBatchMaterial;
            mMeshShader->SetColorUniform(m);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE); // 透明物体は深度書き込み無効（任意）
            batch.gBatchVertexArray->SetActive();
            glDrawElements(GL_TRIANGLES,
                           batch.gBatchVertexArray->GetNumIndices(),
                           GL_UNSIGNED_INT, nullptr);
        }
    }

    for (auto mc : mMeshCompArray)
    {
        // 静的オブジェクトは実行中のみ描画する
        if (GUIEditorManager::IsPlaying())
        {
            if (mc->GetVisible() &&
                mc->GetOwner()->GetStatic() !=
                    ActorInformation::StaticTag::Occluder_Static)
            {
                if (mc->Draw(mMeshShader))
                {
                    mDrawCalls++;
                }
            }
        }
        else
        {
            if (mc->GetVisible())
            {
                if (mc->Draw(mMeshShader))
                {
                    mDrawCalls++;
                }
            }
        }
    }

    // スキンメッシュを有効
    mSkinnedShader->SetActive();
    // ビュー投影行列を更新する
    mSkinnedShader->SetMatrixUniform("uViewProj", view * proj);
    // 照明のユニフォームを更新する
    SetLightUniforms(mSkinnedShader, view);
    for (auto sk : mSkeletalMeshArray)
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
    // パーティクルシステムの描画
    mParticleShader->SetActive();
    // パーティクルで使うため板ポリをアクティブに設定
    mSpriteVerts->SetActive(); // 板ポリ
    mParticleShader->SetMatrixUniform("uViewProj", view * proj);
    for (auto p : mParticlesCompArray)
    {
        if (p->IsVisible())
        {
            p->Draw(mParticleShader);
        }
    }

    glDepthMask(GL_TRUE); // 書き込みを戻す
}

void Renderer::DrawShadow3DScene()
{

    mShadowMap->UpdateLightMatrix(mDirLight.sDirection.Normalized(),
                                  Vector3::Zero);
    Matrix4 lightViewProj = mShadowMap->GetLightViewProj();
    mGGlobalShader->SetActive();
    mGGlobalShader->SetMatrixUniform("uLightViewProj", lightViewProj);

    mShadowMap->BeginRender();

    glClearColor(Color::mClearColor.x, Color::mClearColor.y,
                 Color::mClearColor.z, Color::mClearColor.w);
    glDepthMask(GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    mShadowShader->SetActive();
    mShadowShader->SetMatrixUniform("uLightViewProj", lightViewProj);
    for (auto mc : mMeshCompArray)
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
    for (auto sk : mSkeletalMeshArray)
    {
        if (sk->GetVisible())
        {
            sk->DrawForShadowMap(mSkinnedShadowShader);
        }
    }

    mShadowMap->EndRender();
}

void Renderer::DrawFromGBufferForEditor(SceneViewPanel* scene)
{
    // 描画先を指定されたフレームバッファに切り替える
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
                      scene->GetSceneViewEditor()->GetBufferID());

    glDisable(GL_DEPTH_TEST);

    mGGlobalShader->SetActive();
    mSpriteVerts->SetActive();
    scene->GetSceneBuffer()->SetTexturesActive();
    // シャドウマップを無効にする
    mGGlobalShader->SetBoolUniform(
        "uEnableShadow", scene->IsShadowFrag()); // withShadow = true/false

    if (mSkyBoxRenderer)
    {

        Texture* skyTex = mSkyBoxRenderer->GetTexture();
        if (skyTex)
        {
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyTex->GetTextureID());
        }
    }

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, mShadowMap->GetDepthTexture());

    SetLightUniforms(mGGlobalShader, mView);
    SetPointLightUniforms(mGGlobalShader);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // 深度バッファをコピー（必要に応じて）
    glBindFramebuffer(GL_READ_FRAMEBUFFER,
                      scene->GetSceneBuffer()->GetBufferID());
    int width  = (int)scene->GetSceneWinSize().x;
    int height = (int)scene->GetSceneWinSize().y;
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawFromGBuffer()
{
    // 描画先を指定されたフレームバッファに切り替える
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGameSceneViewEditor->GetBufferID());
    // グローバルライティングパスの深度テストを無効にします
    glDisable(GL_DEPTH_TEST);
    // グローバルGバッファシェーダをアクティブにする
    mGGlobalShader->SetActive();
    // スプライトの頂点クアッドを有効化する
    mSpriteVerts->SetActive();
    // Gバッファーテクスチャをサンプリングするように設定する
    mGBuffer->SetTexturesActive();
    // シャドウマップを有効にする
    mGGlobalShader->SetBoolUniform("uEnableShadow",
                                   true); // withShadow = true/false

    if (mSkyBoxRenderer)
    {

        Texture* skyTex = mSkyBoxRenderer->GetTexture();
        if (skyTex)
        {
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyTex->GetTextureID());
        }
    }

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, mShadowMap->GetDepthTexture());


    // 照明ユニフォームを設定する
    SetLightUniforms(mGGlobalShader, mView);
    SetPointLightUniforms(mGGlobalShader);
    // 三角形を描画
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // Gバッファからデフォルトフレームバッファに深度バッファをコピーする
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer->GetBufferID());
    int width  = static_cast<int>(WindowRenderProperty::GetWidth());
    int height = static_cast<int>(WindowRenderProperty::GetHeight());
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    // 深度テストを有効にしますが、深度バッファへの書き込みを無効にします。
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // ビュー投影行列を設定する
    // mGPointLightShader->SetMatrixUniform("uViewProj",mView * mProjection);
    // サンプリングのためにGバッファーのテクスチャを設定します
    mGBuffer->SetTexturesActive();

    // 点光源の色は既存の色に追加される
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
}

void Renderer::Shutdown()
{
    Texture::UnloadDefaultTextures();

    // メッシュを破壊する
    for (auto i : mMeshesMap)
    {
        i.second->Unload();
        delete i.second;
    }
    mMeshesMap.clear();
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

    if (mGameSceneViewEditor)
    {
        delete mGameSceneViewEditor;
        mGameSceneViewEditor = nullptr;
    }

    // ポイントライトを削除する
    while (!mTypeLights.empty())
    {
        delete mTypeLights.back();
        mTypeLights.pop_back();
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
    if (mSkyBoxRenderer)
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
    // メッシュシェーダーを解放
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
    if (mShadowShader)
    {
        mShadowShader->Unload();
        delete mShadowShader;
        mShadowShader = nullptr;
    }
    // スキンシャドウマップのシェーダーを解放
    if (mSkinnedShadowShader)
    {
        mSkinnedShadowShader->Unload();
        delete mSkinnedShadowShader;
        mSkinnedShadowShader = nullptr;
    }

    // グリッドシェーダーを解放
    if (mGridShader)
    {
        mGridShader->Unload();
        delete mGridShader;
        mGridShader = nullptr;
    }
    // スカイボックスを解放
    if (mSkyBoxShader)
    {
        mSkyBoxShader->Unload();
        delete mSkyBoxShader;
        mSkyBoxShader = nullptr;
    }
    // グリッドを解放
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
    for (auto i : mTexturesMap)
    {
        i.second->Unload();
        delete i.second;
    }
    mTexturesMap.clear();

    for (auto& pair : mAntiTransparentBatchesMap)
    {
        if (pair.second.gBatchVertexArray)
        {
            delete pair.second.gBatchVertexArray;
            pair.second.gBatchVertexArray = nullptr;
        }
    }

    for (auto& pair : mTransparentBatchesMap)
    {
        if (pair.second.gBatchVertexArray)
        {
            delete pair.second.gBatchVertexArray;
            pair.second.gBatchVertexArray = nullptr;
        }
    }
}

void Renderer::AddImageComps(Image* image)
{
    mImageCompArray.emplace_back(image);
}

void Renderer::RemoveImageComp(Image* image)
{
    auto iter =
        std::find(mImageCompArray.begin(), mImageCompArray.end(), image);
    if (iter != mImageCompArray.end())
    {
        mImageCompArray.erase(iter);
    }
}

void Renderer::AddCanvasActor(Canvas* canvas)
{
    mCanvasActorArray.emplace_back(canvas);
}

void Renderer::RemoveCanvasActor(Canvas* canvas)
{
    auto iter =
        std::find(mCanvasActorArray.begin(), mCanvasActorArray.end(), canvas);
    if (iter != mCanvasActorArray.end())
    {
        mCanvasActorArray.erase(iter);
    }
}

void Renderer::AddMeshComp(MeshRenderer* mesh)
{
    if (mesh->GetIsSkeletal())
    {
        SkeletalMeshRenderer* sk = static_cast<SkeletalMeshRenderer*>(mesh);
        mSkeletalMeshArray.emplace_back(sk);
    }
    else
    {
        mMeshCompArray.emplace_back(mesh);
    }
}

void Renderer::RemoveMeshComp(MeshRenderer* mesh)
{
    if (mesh->GetIsSkeletal())
    {
        SkeletalMeshRenderer* sk = static_cast<SkeletalMeshRenderer*>(mesh);
        auto                  iter =
            std::find(mSkeletalMeshArray.begin(), mSkeletalMeshArray.end(), sk);
        if (iter != mSkeletalMeshArray.end())
        {
            mSkeletalMeshArray.erase(iter);
        }
    }
    else
    {
        auto iter =
            std::find(mMeshCompArray.begin(), mMeshCompArray.end(), mesh);
        if (iter != mMeshCompArray.end())
        {
            mMeshCompArray.erase(iter);
        }
    }
}

void Renderer::AddParticleComp(ParticleSystem* particle)
{
    mParticlesCompArray.emplace_back(particle);
}

void Renderer::RemoveParticleComp(ParticleSystem* particle)
{
    auto iter = std::find(mParticlesCompArray.begin(),
                          mParticlesCompArray.end(), particle);
    if (iter != mParticlesCompArray.end())
    {
        mParticlesCompArray.erase(iter);
    }
}

void Renderer::AddPointLight(LocalLight* light)
{
    mTypeLights.emplace_back(light);
}

void Renderer::RemovePointLight(LocalLight* light)
{
    auto iter = std::find(mTypeLights.begin(), mTypeLights.end(), light);
    if (iter != mTypeLights.end())
    {
        mTypeLights.erase(iter);
    }
}

Texture* Renderer::GetTexture(const string& fileName)
{
    Texture* tex  = nullptr;
    auto     iter = mTexturesMap.find(fileName);
    if (iter != mTexturesMap.end())
    {
        tex = iter->second;
    }
    else
    {
        tex = new Texture();
        if (tex->Load(fileName))
        {
            mTexturesMap.emplace(fileName, tex);
        }
        else
        {
            delete tex;
            tex = nullptr;
        }
    }
    return tex;
}
/*
Mesh* Renderer::GetMesh(const string& fileName)
{
    string file = File_P::ModelPath + fileName;
    Mesh*  m    = nullptr;
    auto   iter = mMeshesMap.find(file);
    if (iter != mMeshesMap.end())
    {
        m = iter->second;
    }
    else
    {
        m = new Mesh();
        if (m->LoadFromMeshBin(file, this))
        {
            mMeshesMap.emplace(file, m);
        }
        else if (m->Load(file, this))
        {
            mMeshesMap.emplace(file, m);
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
    // ファイルパス追加
    string filePath = fileName;
    // 返す複数のメッシュ
    vector<class Mesh*> ms;
    // メッシュの数を確認する処理
    Mesh* m     = nullptr;
    m           = new Mesh();
    int maxMesh = m->CheckMeshIndex(filePath, this);
    if (m)
    {
        // メッシュの解放
        m->Unload();
        delete m;
    }
    for (int i = 0; i < maxMesh; i++)
    {
        string inTex = std::to_string(i);
        Mesh*  mesh  = nullptr;
        auto   iter  = mMeshesMap.find(filePath + inTex.c_str());
        // すでに読み込んでいるものならそこから取得
        if (iter != mMeshesMap.end())
        {
            mesh = iter->second;
        }
        else
        {
            mesh = new Mesh();
            // ここにLoad前にバイナリファイルがあるかを確認する
            if (mesh->LoadFromMeshBin(filePath, this, i))
            {
                mMeshesMap.emplace(filePath + inTex.c_str(), mesh);
            }
            else if (mesh->Load(filePath, this, i))
            {
                mMeshesMap.emplace(filePath + inTex.c_str(), mesh);
            }
            else
            {
                delete mesh;
                mesh = nullptr;
            }
        }

        if (mesh != nullptr)
        {
            ms.push_back(mesh);
        }
    }
    return ms;
}
*/

Mesh* Renderer::GetSubMesh(const filesystem::path& fileName, const string& localID)
{
    // キャッシュマップ用のキー(ファイルパス＋localID)
    string cacheKey = fileName.filename().string() + "_" + localID;

    //すでに読み込み済みの場合はキャッシュから返す
    auto iter = mMeshesMap.find(cacheKey);
    if (iter != mMeshesMap.end())
    {
        return iter->second;
    }

    //新規メッシュの作成と読みこみ
    Mesh* mesh = new Mesh();
    //単一サブメッシュ用のロード関数を呼ぶ
    if (mesh->LoadFromSubMesh(fileName.string(), localID))
    {
        mMeshesMap.emplace(cacheKey, mesh);
        return mesh;
    }
    else
    {
        mesh->Unload();
        delete mesh;
        return nullptr;
    }
}

void Renderer::CreateSpriteVerts()
{
    float vertices[] = {
        -0.5f, 0.5f,  0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
        0.5f,  0.5f,  0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
        0.5f,  -0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
        -0.5f, -0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
    };

    unsigned int indices[] = {0, 1, 2, 2, 3, 0};

    mSpriteVerts =
        new VertexArray(vertices, 4, VertexArray::PosNormTex, indices, 6);
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
        {Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f)},
        {Vector3(1.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f)},

        // Y軸（緑）
        {Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)},
        {Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)},

        // Z軸（青）
        {Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f)},
        {Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f)},
    };
    mAxisVAO = new VertexArray(axisVerts);
}

void Renderer::SetPointLightUniforms(Shader* shader)
{
    const int MAX_POINT_LIGHTS = 20;
    int       lightCount       = 0;

    vector<int>     type;
    vector<Vector3> positions;
    vector<Vector3> direction;
    vector<Vector3> colors;
    vector<float>   ranges;
    vector<Vector2> angles;

    for (auto light : mTypeLights)
    {
        if (!light->IsRun())
            continue;
        if (lightCount >= MAX_POINT_LIGHTS)
            break;
        type.push_back((int)light->GetLightType());
        positions.push_back(
            light->GetOwner()->GetBaseTransform()->GetPosition());
        direction.push_back(
            light->GetOwner()->GetBaseTransform()->GetForward());
        colors.push_back(light->GetColor());
        ranges.push_back(light->GetRange());
        angles.push_back(light->GetAngles());

        lightCount++;
    }

    shader->SetIntUniform("uNumLights", lightCount);

    for (int i = 0; i < lightCount; ++i)
    {
        string baseName = "uLights[" + std::to_string(i) + "].";

        shader->SetIntUniform((baseName + "sType").c_str(), type[i]);
        shader->SetVectorUniform((baseName + "sPosition").c_str(),
                                 positions[i]);
        shader->SetVectorUniform((baseName + "sDirection").c_str(),
                                 direction[i]);
        shader->SetVectorUniform((baseName + "sColor").c_str(), colors[i]);
        shader->SetFloatUniform((baseName + "sRange").c_str(), ranges[i]);
        shader->SetVector2Uniform((baseName + "sAngles").c_str(), angles[i]);
    }
}

void Renderer::SetLightUniforms(Shader* shader, const Matrix4& view)
{
    // カメラの位置は逆さまの視点からです
    Matrix4 invView = view;
    invView.Invert();
    shader->SetVectorUniform("uCameraPos", invView.GetTranslation());
    // Ambient light
    shader->SetVectorUniform("uAmbientLight", mDirLight.sAmbientColor);
    shader->SetFloatUniform("uAmbientIntensity", mDirLight.sAmbientIntensity);
    // Directional light
    shader->SetVectorUniform("uDirLight.mDirection", mDirLight.sDirection);
    shader->SetVectorUniform("uDirLight.mDiffuseColor",
                             mDirLight.sDiffuseColor);
    shader->SetVectorUniform("uDirLight.mSpecColor", mDirLight.sSpecColor);
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
    Vector3 end   = Unproject(screenPoint);
    // 方向ベクトルを取得する
    outDir = end - outStart;
    outDir.Normalize();
}
