#pragma once
#include "SDL3.h"
#include "Typedefs.h"
#include "GameWinMain.h"
#include "EngineWindow.h"
#include "GameApp.h"
/*
* ===�G���W����������/Engine internal processing===
*/

//���Ќ������������t�@�C��
//�����̍\����
struct DirectionalLightData
{
	// Direction of light
	Vector3 mDirection = Vector3();
	// Diffuse color
	Vector3 mDiffuseColor = Vector3();
	// Ambient color
	Vector3 mAmbientColor = Vector3();
	// Specular color
	Vector3 mSpecColor = Vector3();
	//�ʒu
	Vector3 mPosition = Vector3();
};

class BaseScene;
class Shader;
class Texture;
class VertexArray;
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
// 3D�`��̃����_���[
//�Q�[���̃����_�����O��S������N���X
class Renderer
{
private:
	// BaseScene
	BaseScene*											mNowScene;
	// �e�N�X�`���̃}�b�v���ǂݍ��ݕϐ�
	std::unordered_map<string,Texture*>					mTextures;
	// Sprite shader
	Shader*												mSpriteShader;
	// Sprite vertex array
	VertexArray*										mSpriteVerts;
	//2D�摜�p�̒��_�z��
	VertexArray*										mFanSpriteVerts;
	//�p�[�e�B�N���V�X�e���̔z��
	vector<ParticleSystem*>								mParticlesComps;
	//�p�[�e�B�N���V�F�[�_�[
	Shader*												mParticleShader;
	// ���b�V���̒n�}�����[�h
	std::unordered_map<string,Mesh*>					mMeshes;
	// ���ׂẮi���i�ȊO�́j���b�V���R���|�[�l���g
	vector<MeshRenderer*>								mMeshComps;
	vector<SkeletalMeshRenderer*>						mSkeletalMeshes;
	// Mesh shader
	Shader*												mMeshShader;
	// Skinned shader
	Shader*												mSkinnedShader;
	// View/projection for 3D shaders
	Matrix4												mView;
	Matrix4												mProjection;
	//�����̃f�[�^�\����
	DirectionalLightData								mDirLight;
	// Window
	SDL_Window*											mWindow;
	// OpenGL context
	SDL_GLContext										mContext;
	//GBuffer�N���X
	GBuffer*											mGBuffer;
	GBuffer*											mSceneBuffer;
	// GBuffer shader
	Shader*												mGGlobalShader;
	//�V���h�E�}�b�v�̃N���X
	ShadowMap*											mShadowMap;
	Shader*												mShadowShader;
	Shader*												mSkinnedShadowShader;
	//�|�C���g���C�g�̔z��
	vector<PointLightComponent*>						mPointLights;
	Shader*												mGPointLightShader;
	Mesh*												mPointLightMesh;
	//�X�J�C�{�b�N�X�̃����_���[
	SkyBoxRenderer*										mSkyBoxRenderer;
	Shader*												mSkyBoxShader;
	//�f�o�b�O�O���b�h�̃|�C���^�N���X
	DebugGrid*											mDebugGrid;
	//�O���b�h�̃V�F�[�_�[
	Shader*												mGridShader;
	Shader*												mArrowShader;
	//�I�u�W�F�N�g�̕������p�̒��_�z��
	VertexArray*										mAxisVAO;

	SceneViewEditor*									mSceneViewEditor;
	SceneViewEditor*									mGameSceneViewEditor;

	//3D�`�揈��
	void												EditorDraw3DScene(unsigned int framebuffer, const Matrix4& view, const Matrix4& proj,
		float viewPortScale = 1.0f, bool lit = true);
	//3D�`�揈��
	void												Draw3DScene(unsigned int framebuffer, const Matrix4& view, const Matrix4& proj,
		float viewPortScale = 1.0f, bool lit = true);
	void												DrawShadow3DScene();

	void												DrawFromGBufferForEditor();
	//���C�g�`�揈��
	void												DrawFromGBuffer();
	//Shader�̓ǂݍ���
	bool												LoadShaders();
	//Sprite�̒��_���쐬
	void												CreateSpriteVerts();
	//��^�X�v���C�g�̒��_���쐬
	int 												CreateFanSpriteVerts(float fillRatio /*0.0�`1.0: ��̊���*/, int segments);
	//�I�u�W�F�N�g�̕������̒��_���쐬
	void 												CreateAxisVerts();

	//���C�g��Shader�A�}�g���b�N�X��Setter
	void												SetLightUniforms(class Shader* shader, const Matrix4& view);
public:
														Renderer();
														~Renderer();

	bool												Initialize(float screenWidth, float screenHeight);
	//�`�敔���̃A�����[�h(Shader�Ȃ�)
	void												Shutdown();
	//�V�[���ʂɕۑ����Ă���I�u�W�F�N�g���A�����[�h���鏈��
	void												UnloadData();
	void												MeshOrderUpdate();
	//�`�揈��
	void												StartDraw();
	void												EndDraw();
	//Mesh�ǉ�����
	void												AddMeshComp(class MeshRenderer* mesh);
	//Mesh�폜����
	void												RemoveMeshComp(class MeshRenderer* mesh);
	//Mesh�ǉ�����
	void												AddParticleComp(class ParticleSystem* particle);
	//Mesh�폜����
	void												RemoveParticleComp(class ParticleSystem* particle);
	//PointLight�ǉ�����
	void												AddPointLight(class PointLightComponent* light);
	//PointLight�폜����
	void												RemovePointLight(class PointLightComponent* light);
	// Given a screen space point, unprojects it into world space,
	// based on the current 3D view/projection matrices
	// Expected ranges:
	// x = [-screenWidth/2, +screenWidth/2]
	// y = [-screenHeight/2, +screenHeight/2]
	// z = [0, 1) -- 0 is closer to camera, 1 is further
	// �X�N���[�����W���烏�[���h���W�ւ̕ϊ�
	Vector3												Unproject(const Vector3& screenPoint) const;
	//�X�J�C�{�b�N�X��Getter
	SkyBoxRenderer*										GetSkyBoxRenderer() { return mSkyBoxRenderer; }
	//PointLightMesh�̐ݒ菈��
	void												SetPointLightMesh(class Mesh* mesh) { mPointLightMesh = mesh; }
	//Texture��Getter
	Texture*											GetTexture(const string& fileName);
	//Mesh��Getter
	Mesh*												GetMesh(const string& fileName);
	//Mesh�z���Getter
	vector<class Mesh*>									GetMeshs(const string& fileName);

	Matrix4												GetView() { return mView; }
	//�J�����̃r���[�s���Setter
	void												SetViewMatrix(const Matrix4& view) { mView = view; }
	//DirLight��Getter
	DirectionalLightData								GetDirectionalLight() { return mDirLight; }
	//DirLight��Setter
	void												SetDirectionalLight(DirectionalLightData dirLight) { mDirLight = dirLight; }
	//�X�N���[���̕������擾
	void												GetScreenDirection(Vector3& outStart, Vector3& outDir) const;
	//GBuffer��Getter
	GBuffer*											GetGBuffer() { return mGBuffer; }
	GBuffer*											GetSceneBuffer() { return mSceneBuffer; }
	// Mesh shader
	Shader*												GetMeshShader() { return mMeshShader; }
	// Skinned shader
	Shader*												GetSkinnedShader() { return mSkinnedShader; }
	//BaseScene��Getter
	void												SetBaseScene(class BaseScene* scene) { mNowScene = scene; }
	//mWindow��Getter
	SDL_Window*											GetWindow() { return mWindow; }
	// mContext��Getter
	SDL_GLContext										GetContext() { return mContext; }

	ShadowMap*											GetShadowMap() { return mShadowMap; }

	// mSceneViewEditor��Getter
	SceneViewEditor*									GetSceneViewEditor() { return mSceneViewEditor; }
	// mGameSceneViewEditor��Getter
	SceneViewEditor*									GetGameSceneViewEditor() { return mGameSceneViewEditor; }
};