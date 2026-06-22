#pragma once
#include "SDL3.h"
#include "Typedefs.h"
#include "VertexArray.h"
#include "Texture.h"
#include "BaseScene.h"
#include "Shader.h"
#include "SelectionManager.h"
#include "SkyBoxRenderer.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/

//環境光の構造体
struct DirectionalLightData
{
	// Direction of light
	Vector3 sDirection = Vector3();
	// Diffuse color
	Vector3 sDiffuseColor = Vector3();
	// Ambient color
	Vector3 sAmbientColor = Vector3();
	float	sAmbientIntensity = 1.0f;
	// Specular color
	Vector3 sSpecColor = Vector3();
	//位置
	Vector3 sPosition = Vector3();
};

struct PointLightGPUData
{
	Vector3		sPosition = Vector3();
	float		sRange = 0;
	Vector3		sColor = Vector3();
	float		sPadding = 0;
};

struct StaticMeshBatch {
	vector<Vertex>			gAllVertices;
	vector<unsigned int>	gAllIndices;
	VertexArray*			gBatchVertexArray = nullptr;
	Texture*				gBatchTexture = nullptr;
	MaterialInfo 			gBatchMaterial;
};

class ParticleSystem;
class Mesh;
class MeshRenderer;
class SkeletalMeshRenderer;
class GBuffer;
class PointLightComponent;
class ShadowMap;
class SkyBoxRenderer;
class DebugGrid;
class SceneViewEditor;
// 3D描画のレンダラー
//ゲームのレンダリングを担当するクラス
class Renderer
{
private:
	string												mWindowTitle;
	// BaseScene
	BaseScene*											mRunScene;
	// テクスチャのマップが読み込み変数
	std::unordered_map<string,Texture*>					mTexturesMap;
	vector<Image*>										mImageCompArray;
	vector<Canvas*>										mCanvasActorArray;
	// Sprite shader
	Shader*												mSpriteShader;
	// Sprite vertex array
	VertexArray*										mSpriteVerts;
	//2D画像用の頂点配列
	VertexArray*										mFanSpriteVerts;
	//パーティクルシステムの配列
	vector<ParticleSystem*>								mParticlesCompArray;
	//パーティクルシェーダー
	Shader*												mParticleShader;
	// メッシュの地図がロード
	std::unordered_map<string,Mesh*>					mMeshesMap;
	// すべての（骨格以外の）メッシュコンポーネント
	vector<MeshRenderer*>								mMeshCompArray;
	vector<SkeletalMeshRenderer*>						mSkeletalMeshArray;
	// Mesh shader
	Shader*												mMeshShader;
	// Skinned shader
	Shader*												mSkinnedShader;
	// View/projection for 3D shaders
	Matrix4												mView;
	Matrix4												mProjection;
	//環境光のデータ構造体
	DirectionalLightData								mDirLight;
	// Window
	SDL_Window*											mWindow;
	// OpenGL context
	SDL_GLContext										mContext;
	//GBufferクラス
	GBuffer*											mGBuffer;

	// GBuffer shader
	Shader*												mGGlobalShader;

	vector<PointLightGPUData>							mLightDataArray;

	//シャドウマップのクラス
	ShadowMap*											mShadowMap;
	Shader*												mShadowShader;
	Shader*												mSkinnedShadowShader;

	//ポイントライトの配列
	vector<PointLightComponent*>						mPointLights;

	Mesh*												mPointLightMesh;
	//スカイボックスのレンダラー
	SkyBoxRenderer*										mSkyBoxRenderer;
	Shader*												mSkyBoxShader;
	//デバッググリッドのポインタクラス
	DebugGrid*											mDebugGrid;
	//グリッドのシェーダー
	Shader*												mGridShader;
	Shader*												mArrowShader;
	//オブジェクトの方向矢印用の頂点配列
	VertexArray*										mAxisVAO;

	//ゲームシーンのデータ
	SceneViewEditor*									mGameSceneViewEditor;
	//シーンごとに保存しているオブジェクトのバッチ
	// アンチ半透明バッチ
	std::map<MaterialInfo*, StaticMeshBatch>			mAntiTransparentBatchesMap;      
	// 半透明バッチ
	std::map<MaterialInfo*, StaticMeshBatch>			mTransparentBatchesMap; 

	//描画回数のカウンター
	int													mDrawCalls;

	//3D描画処理
	void												EditorDraw3DScene(class SceneViewPanel* scene,unsigned int framebuffer, const Matrix4& view, const Matrix4& proj,
		float viewPortScale = 1.0f, bool lit = true);
	//3D描画処理
	void												Draw3DScene(unsigned int framebuffer, const Matrix4& view, const Matrix4& proj,
		float viewPortScale = 1.0f, bool lit = true);
	void												DrawShadow3DScene();

	void												DrawFromGBufferForEditor(class SceneViewPanel* scene);
	//ライト描画処理
	void												DrawFromGBuffer();
	//Shaderの読み込み
	bool												LoadShaders();
	//Spriteの頂点を作成
	void												CreateSpriteVerts();
	//扇型スプライトの頂点を作成
	int 												CreateFanSpriteVerts(float fillRatio /*0.0～1.0: 扇の割合*/, int segments);
	//オブジェクトの方向矢印の頂点を作成
	void 												CreateAxisVerts();

	void												SetPointLightUniforms(Shader* shader);

	//ライトのShader、マトリックスのSetter
	void												SetLightUniforms(class Shader* shader, const Matrix4& view);
public:
														Renderer();
														~Renderer();

	bool												Initialize(float screenWidth, float screenHeight);
	//ゲーム実行時に一度だけ呼び出される初期化処理
	void												BuildStaticBatch();

	void 												BuildMeshBatch(Mesh* mesh, Matrix4 world, StaticMeshBatch& outBatch, int index);

	//描画部分のアンロード(Shaderなど)
	void												Shutdown();
	//シーン別に保存しているオブジェクトをアンロードする処理
	void												UnloadData();
	void												MeshOrderUpdate();
	void												DrawWindowTitle();
	//描画処理
	void												StartDraw();
	void												EndDraw();

	void												AddImageComps(Image* image);
	void												RemoveImageComp(Image* image);

	void												AddCanvasActor(Canvas* canvas);
	void												RemoveCanvasActor(Canvas* canvas);

	//Mesh追加処理
	void												AddMeshComp(class MeshRenderer* mesh);
	//Mesh削除処理
	void												RemoveMeshComp(class MeshRenderer* mesh);
	//Mesh追加処理
	void												AddParticleComp(class ParticleSystem* particle);
	//Mesh削除処理
	void												RemoveParticleComp(class ParticleSystem* particle);
	//PointLight追加処理
	void												AddPointLight(class PointLightComponent* light);
	//PointLight削除処理
	void												RemovePointLight(class PointLightComponent* light);
	// Given a screen space point, unprojects it into world space,
	// based on the current 3D view/projection matrices
	// Expected ranges:
	// x = [-screenWidth/2, +screenWidth/2]
	// y = [-screenHeight/2, +screenHeight/2]
	// z = [0, 1) -- 0 is closer to camera, 1 is further
	// スクリーン座標からワールド座標への変換
	Vector3												Unproject(const Vector3& screenPoint) const;
	//スカイボックスのGetter
	SkyBoxRenderer*										GetSkyBoxRenderer() { return mSkyBoxRenderer; }
	//PointLightMeshの設定処理
	void												SetPointLightMesh(class Mesh* mesh) { mPointLightMesh = mesh; }
	//TextureのGetter
	Texture*											GetTexture(const string& fileName);
	//MeshのGetter
	Mesh*												GetMesh(const string& fileName);
	//Mesh配列のGetter
	vector<class Mesh*>									GetMeshs(const string& fileName);

	Matrix4												GetView() { return mView; }
	//カメラのビュー行列のSetter
	void												SetViewMatrix(const Matrix4& view) { mView = view; }
	//DirLightのGetter
	DirectionalLightData								GetDirectionalLight() { return mDirLight; }
	//DirLightのSetter
	void												SetDirectionalLight(DirectionalLightData dirLight) { mDirLight = dirLight; }
	//スクリーンの方向を取得
	void												GetScreenDirection(Vector3& outStart, Vector3& outDir) const;
	//GBufferのGetter
	GBuffer*											GetGBuffer() { return mGBuffer; }
	// Mesh shader
	Shader*												GetMeshShader() { return mMeshShader; }
	// Skinned shader
	Shader*												GetSkinnedShader() { return mSkinnedShader; }

	class BaseScene*									GetRunScene() { return mRunScene; }
	//BaseSceneのGetter
	void												SetBaseScene(class BaseScene* scene) { mRunScene = scene; }
	//mWindowのGetter
	SDL_Window*											GetWindow() { return mWindow; }
	// mContextのGetter
	SDL_GLContext										GetContext() { return mContext; }

	ShadowMap*											GetShadowMap() { return mShadowMap; }

	// mGameSceneViewEditorのGetter
	SceneViewEditor*									GetGameSceneViewEditor() { return mGameSceneViewEditor; }

	int 												GetDrawCalls() { return mDrawCalls; }
};