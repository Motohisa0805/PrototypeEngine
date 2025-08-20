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
	// OpenGL�̑�����ݒ肷��
	// �R�AOpenGL�v���t�@�C�����g�p
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// �o�[�W����3.3���w��
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// RGBA�`���l�����Ƃ�8�r�b�g�̃J���[�o�b�t�@�����N�G�X�g
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	// �_�u���o�b�t�@�����O��L���ɂ���
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// OpenGL�Ƀn�[�h�E�F�A�A�N�Z�����[�V�������g�p
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	//SDL_Window���쐬����
	mWindow = SDL_CreateWindow("SDL&OpenGLProject",static_cast<int>(WindowRenderProperty::GetWidth()), static_cast<int>(WindowRenderProperty::GetHeight()), SDL_WINDOW_OPENGL);
	//�G���[�`�F�b�N
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	// OpenGL�R���e�L�X�g���쐬����
	mContext = SDL_GL_CreateContext(mWindow);
	// GLEW������������
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("Failed to initialize GLEW.");
		return false;
	}
	// �ꕔ�̃v���b�g�t�H�[���ł́AGLEW�����Q�ȃG���[�R�[�h���o�͂���̂ŁA
	// ������N���A�B
	glGetError();
	// �`��p��2D��`���쐬����
	CreateSpriteVerts();
	//�I�u�W�F�N�g�̕������p�̒��_�z����쐬
	CreateAxisVerts();

	//�X�J�C�{�b�N�X�𐶐�
	mSkyBoxRenderer = new SkyBoxRenderer();

	//�f�o�b�O�O���b�h����
	mDebugGrid = new DebugGrid();
	// G�o�b�t�@���쐬����
	mGBuffer = new GBuffer();
	int width = static_cast<int>(WindowRenderProperty::GetWidth());
	int height = static_cast<int>(WindowRenderProperty::GetHeight());
	if (!mGBuffer->Create(width, height))
	{
		SDL_Log("Failed to create G-buffer.");
		return false;
	}
	//�V���h�E�}�b�v���쐬����
	mShadowMap = new ShadowMap();
	if(!mShadowMap->Initialize(width, height))
	{
		SDL_Log("Failed to create shadow map.");
		return false;
	}
	// �V�F�[�_�[���쐬/�R���p�C���ł��邱�Ƃ��m�F���Ă�������
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

	// �X�v���C�g�V�F�[�_�[���쐬����
	mSpriteShader = new Shader();
	if (!mSpriteShader->Load("Sprite.vert", "Sprite.frag"))
	{
		return false;
	}
	mSpriteShader->SetActive();
	Matrix4 spriteViewProj = Matrix4::CreateSimpleViewProj(WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight());
	// �r���[���e�s���ݒ肷��
	mSpriteShader->SetMatrixUniform("uViewProj", spriteViewProj);

	// �r���[���e�s���ݒ肷��
	mView = Matrix4::CreateLookAt(WindowRenderProperty::GetViewEye(), WindowRenderProperty::GetViewTarget(), WindowRenderProperty::GetViewUp());
	mProjection = Matrix4::CreatePerspectiveFOV(Math::ToRadians(WindowRenderProperty::GetFieldOfView()), WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight(), WindowRenderProperty::GetCameraNear(), WindowRenderProperty::GetCameraFar());

	// ��{�I�ȃ��b�V���V�F�[�_�[���쐬����
	mMeshShader = new Shader();
	if (!mMeshShader->Load("Phong.vert", "GBufferWrite.frag"))
	{
		return false;
	}
	mMeshShader->SetActive();
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);

	// �X�L���V�F�[�_�[���쐬����
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

	// �p�[�e�B�N���V�F�[�_�[���쐬����
	mParticleShader = new Shader();
	if (!mParticleShader->Load("Sprite.vert", "ParticleSprite.frag"))
	{
		return false;
	}
	mParticleShader->SetActive();
	mParticleShader->SetMatrixUniform("uViewProj", mView * mProjection);

	// GBuffer����`�悷�邽�߂̃V�F�[�_�[���쐬����i�O���[�o�����C�e�B���O�j
	mGGlobalShader = new Shader();
	if (!mGGlobalShader->Load("GBufferGlobal.vert", "GBufferGlobal.frag"))
	{
		return false;
	}
	// GBuffer�̂��߂ɁA�e�T���v���[���C���f�b�N�X�Ɋ֘A�t����
	mGGlobalShader->SetActive();
	mGGlobalShader->SetIntUniform("uGDiffuse", 0);
	mGGlobalShader->SetIntUniform("uGNormal", 1);
	mGGlobalShader->SetIntUniform("uGWorldPos", 2);
	// �r���[���e�͂����̃X�v���C�g�̂��̂ł�
	mGGlobalShader->SetMatrixUniform("uViewProj", spriteViewProj);
	// ���E�̕ό`�X�P�[������ʂɓK�p����Ay�����]���܂�
	Matrix4 gbufferWorld = Matrix4::CreateScale(WindowRenderProperty::GetWidth(), -WindowRenderProperty::GetHeight(),
		1.0f);
	mGGlobalShader->SetMatrixUniform("uWorldTransform", gbufferWorld);
	glActiveTexture(GL_TEXTURE3); // �󂢂Ă���e�N�X�`�����j�b�g��I��
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

	// GBuffer����|�C���g���C�g�p�̃V�F�[�_�[���쐬����
	mGPointLightShader = new Shader();
	if (!mGPointLightShader->Load("BasicMesh.vert","GBufferPointLight.frag"))
	{
		return false;
	}
	// �T���v���[�C���f�b�N�X��ݒ肷��
	mGPointLightShader->SetActive();
	mGPointLightShader->SetIntUniform("uGDiffuse", 0);
	mGPointLightShader->SetIntUniform("uGNormal", 1);
	mGPointLightShader->SetIntUniform("uGWorldPos", 2);
	mGPointLightShader->SetVector2Uniform("uScreenDimensions",Vector2(WindowRenderProperty::GetWidth(), WindowRenderProperty::GetHeight()));
	//�O���b�h��`�悷�邽�߂̃V�F�[�_�[���쐬����
	mGridShader = new Shader();
	if (!mGridShader->Load("Grid.vert", "Grid.frag"))
	{
		return false;
	}
	return true;
}

void Renderer::MeshOrderUpdate()
{
	// 1. �ꎞ���X�g�ɕ���
	std::vector<MeshRenderer*> opaqueList;
	std::vector<MeshRenderer*> transparentList;
	// �����I�u�W�F�N�g�ƕs�����I�u�W�F�N�g�𕪂���
	for (auto& mesh : mMeshComps)
	{
		if (!mesh->GetVisible()) continue;
		//MeshRenderer����Mesh��1���`�F�b�N
		for (auto& m : mesh->GetMeshs())
		{
			// nullptr�`�F�b�N
			if (!m) continue; 
			// �}�e���A�����Ȃ��ꍇ�̓X�L�b�v
			if (m->GetMaterialInfo().empty()) continue; 
			const auto& materials = m->GetMaterialInfo();
			bool isTransparent = false;
			for (const auto& mat : materials)
			{
				// �s�����x��1�����Ȃ瓧���Ƃ݂Ȃ�
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

	// 2. �����I�u�W�F�N�g�̓J��������̋����Ń\�[�g�i�������j
	Matrix4 view = mView;
	view.Invert();
	// �r���[�s��̋t�s�񂩂�J�����ʒu�擾
	Vector3 cameraPos = view.GetTranslation(); 
	std::sort(transparentList.begin(), transparentList.end(),
		[&](MeshRenderer* a, MeshRenderer* b)
		{
			float distA = (a->GetOwner()->GetPosition() - cameraPos).LengthSq();
			float distB = (b->GetOwner()->GetPosition() - cameraPos).LengthSq();
			return distA > distB; // ��������
		}
	);
	// 3. mMeshComps ���č\�z
	mMeshComps.clear();
	mMeshComps.insert(mMeshComps.end(), opaqueList.begin(), opaqueList.end());
	mMeshComps.insert(mMeshComps.end(), transparentList.begin(), transparentList.end());
}

void Renderer::Draw()
{
	//Mesh�̏��Ԃ�ύX
	MeshOrderUpdate();

	// ���C�g���_�Ő[�x�����V���h�E�}�b�v�ɕ`��
	DrawShadow3DScene();

	// G-buffer��3D�V�[����`�悵�܂��B
	Draw3DScene(mGBuffer->GetBufferID(), mView, mProjection, 1.0f, true);

	Draw3DScene(mSceneViewEditor->GetSceneFBO(), mView, mProjection, 1.0f, true);
	// �t���[���o�b�t�@���[���i�X�N���[���̃t���[���o�b�t�@�j�ɖ߂��܂�
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// G�o�b�t�@����`�悷��
	DrawFromGBuffer();

	// ���ׂẴX�v���C�g�R���|�[�l���g��`�悷��
	// �[�x�o�b�t�@�����O�𖳌��ɂ���
	glDisable(GL_DEPTH_TEST);
	// �J���[ �o�b�t�@�ŃA���t�@ �u�����f�B���O��L���ɂ��܂�
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// �V�F�[�_�[/VAO���A�N�e�B�u�ɐݒ�
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

	// �o�b�t�@�����ւ���
	SDL_GL_SwapWindow(mWindow);
}

void Renderer::Draw3DScene(unsigned int framebuffer, const Matrix4& view, const Matrix4& proj,float viewPortScale, bool lit)
{
	// ���݂̃t���[���o�b�t�@��ݒ肷��
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// �X�P�[���Ɋ�Â��ăr���[�|�[�g�T�C�Y��ݒ肵�܂�
	glViewport(0, 0,static_cast<int>(WindowRenderProperty::GetWidth() * viewPortScale),static_cast<int>(WindowRenderProperty::GetHeight() * viewPortScale));
	
	// �J���[ �o�b�t�@/�[�x�o�b�t�@���N���A
	glClearColor(Color::mClearColor.x, Color::mClearColor.y, Color::mClearColor.z, Color::mClearColor.w);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// �X�J�C�{�b�N�X�`��
	mSkyBoxRenderer->Draw(mSkyBoxShader, view, proj);

	// ���b�V���R���|�[�l���g��`�悷��[�x�o�b�t�@�����O��L���ɂ���
	// �A���t�@�u�����h�𖳌��ɂ���
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// ���b�V���i�ÓI�j
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

	// �X�L�����b�V����L��
	mSkinnedShader->SetActive();
	// �r���[���e�s����X�V����
	mSkinnedShader->SetMatrixUniform("uViewProj", view * proj);
	// �Ɩ��̃��j�t�H�[�����X�V����
	SetLightUniforms(mSkinnedShader, view);
	for (auto sk : mSkeletalMeshes)
	{
		if (sk->GetVisible())
		{
			sk->Draw(mSkinnedShader);
		}
	}
	// 2. �p�[�e�B�N���Ȃǔ��������̂�`��
	//  Z��r��L���i�K�{�j
	glEnable(GL_DEPTH_TEST);                     
	//  Z�o�b�t�@�������݂�h��
	glDepthMask(GL_FALSE);                       
	//  ���ߍ���
	glEnable(GL_BLEND);                          
	// �A���t�@�u�����h
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	//�p�[�e�B�N���V�X�e���̕`��
	mParticleShader->SetActive();
	//�p�[�e�B�N���Ŏg�����ߔ|�����A�N�e�B�u�ɐݒ�
	mSpriteVerts->SetActive(); // �|��
	mParticleShader->SetMatrixUniform("uViewProj", view * proj);
	for(auto p : mParticlesComps)
	{
		if (p->IsVisible())
		{
			p->Draw(mParticleShader);
		}
	}
	//�f�o�b�O�`��
	if (GameStateClass::mDebugFrag)
	{
		mArrowShader->SetActive();
		mArrowShader->SetMatrixUniform("uViewProj", view * proj);
		for(auto actor : mNowScene->GetActors())
		{
			if (actor->GetState() == ActorObject::EActive)
			{
				// �A�N�^�[�̃f�o�b�O�`��
				mArrowShader->SetMatrixUniform("uModel", actor->GetWorldTransform());

				mAxisVAO->SetActive(); // 6���_�i3�� �~ 2�_�j
				glLineWidth(3.0f); // ���̑�����3�s�N�Z���ɐݒ�
				glDrawArrays(GL_LINES, 0, 6);
			}
		}
		mDebugGrid->Draw(mGridShader, view * proj);
	}

	glDepthMask(GL_TRUE);  // �������݂�߂�
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
	// �X�L�����b�V����L��
	mSkinnedShadowShader->SetActive();
	// �r���[���e�s����X�V����
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
	// �O���[�o�����C�e�B���O�p�X�̐[�x�e�X�g�𖳌��ɂ��܂�
	glDisable(GL_DEPTH_TEST);
	// �O���[�o��G�o�b�t�@�V�F�[�_���A�N�e�B�u�ɂ���
	mGGlobalShader->SetActive();
	// �X�v���C�g�̒��_�N�A�b�h��L��������
	mSpriteVerts->SetActive();
	// G�o�b�t�@�[�e�N�X�`�����T���v�����O����悤�ɐݒ肷��
	mGBuffer->SetTexturesActive();

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mShadowMap->GetDepthTexture());
	mGGlobalShader->SetIntUniform("uShadowMap", 3);

	// �Ɩ����j�t�H�[����ݒ肷��
	SetLightUniforms(mGGlobalShader, mView);
	// �O�p�`��`��
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	// G�o�b�t�@����f�t�H���g�t���[���o�b�t�@�ɐ[�x�o�b�t�@���R�s�[����
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer->GetBufferID());
	int width = static_cast<int>(WindowRenderProperty::GetWidth());
	int height = static_cast<int>(WindowRenderProperty::GetHeight());
	glBlitFramebuffer(0, 0, width, height,0, 0, width, height,GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	// �[�x�e�X�g��L���ɂ��܂����A�[�x�o�b�t�@�ւ̏������݂𖳌��ɂ��܂��B
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	/*
	// �_�����V�F�[�_�[�ƃ��b�V�����A�N�e�B�u�ɐݒ肵�܂��B
	mGPointLightShader->SetActive();
	size_t size = mPointLightMesh->GetVertexArrays().size();
	for (unsigned int i = 0; i < size; i++) 
	{
		mPointLightMesh->GetVertexArrays()[i]->SetActive();
	}
	*/
	// �r���[���e�s���ݒ肷��
	//mGPointLightShader->SetMatrixUniform("uViewProj",mView * mProjection);
	// �T���v�����O�̂��߂�G�o�b�t�@�[�̃e�N�X�`����ݒ肵�܂�
	mGBuffer->SetTexturesActive();

	// �_�����̐F�͊����̐F�ɒǉ������
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	/*
	// �|�C���g���C�g��`��
	for (PointLightComponent* p : mPointLights)
	{
		p->Draw(mGPointLightShader, mPointLightMesh);
	}
	*/
}

void Renderer::Shutdown()
{
	// G�o�b�t�@����菜��
	if (mGBuffer)
	{
		mGBuffer->Destroy();
		delete mGBuffer;
		mGBuffer = nullptr;
	}
	// �V���h�E�}�b�v����菜��
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

	// �|�C���g���C�g���폜����
	while (!mPointLights.empty())
	{
		delete mPointLights.back();
		mPointLights.pop_back();
	}

	// ���_�z��
	if (mSpriteVerts)
	{
		delete mSpriteVerts;
		mSpriteVerts = nullptr;
	}
	// 2D�摜�p�̒��_�z������
	if (mFanSpriteVerts)
	{
		delete mFanSpriteVerts;
		mFanSpriteVerts = nullptr;
	}
	// �I�u�W�F�N�g�̕������p�̒��_�z������
	if (mAxisVAO)
	{
		delete mAxisVAO;
		mAxisVAO = nullptr;
	}
	// �X�J�C�{�b�N�X�����_���[�����
	if( mSkyBoxRenderer)
	{
		delete mSkyBoxRenderer;
		mSkyBoxRenderer = nullptr;
	}
	// �X�v���C�g�V�F�[�_�[
	if (mSpriteShader)
	{
		mSpriteShader->Unload();
		delete mSpriteShader;
		mSpriteShader = nullptr;
	}
	//���b�V���V�F�[�_�[�����
	if (mMeshShader)
	{
		mMeshShader->Unload();
		delete mMeshShader;
		mMeshShader = nullptr;
	}
	// �X�L�����b�V���V�F�[�_�[�����
	if (mSkinnedShader)
	{
		mSkinnedShader->Unload();
		delete mSkinnedShader;
		mSkinnedShader = nullptr;
	}
	// ���V�F�[�_�[�����
	if (mArrowShader)
	{
		mArrowShader->Unload();
		delete mArrowShader;
		mArrowShader = nullptr;
	}
	// �p�[�e�B�N���V�F�[�_�[�����
	if (mParticleShader)
	{
		mParticleShader->Unload();
		delete mParticleShader;
		mParticleShader = nullptr;
	}
	// G�o�b�t�@�[�̃V�F�[�_�[�����
	if (mGGlobalShader)
	{
		mGGlobalShader->Unload();
		delete mGGlobalShader;
		mGGlobalShader = nullptr;
	}
	// �V���h�E�}�b�v�̃V�F�[�_�[�����
	if(mShadowShader)
	{
		mShadowShader->Unload();
		delete mShadowShader;
		mShadowShader = nullptr;
	}
	// �X�L���V���h�E�}�b�v�̃V�F�[�_�[�����
	if(mSkinnedShadowShader)
	{
		mSkinnedShadowShader->Unload();
		delete mSkinnedShadowShader;
		mSkinnedShadowShader = nullptr;
	}
	// G�o�b�t�@�[�̃|�C���g���C�g�V�F�[�_�[�����
	if (mGPointLightShader)
	{
		mGPointLightShader->Unload();
		delete mGPointLightShader;
		mGPointLightShader = nullptr;
	}
	// �O���b�h�V�F�[�_�[�����
	if (mGridShader)
	{
		mGridShader->Unload();
		delete mGridShader;
		mGridShader = nullptr;
	}
	//�X�J�C�{�b�N�X�����
	if( mSkyBoxShader)
	{
		mSkyBoxShader->Unload();
		delete mSkyBoxShader;
		mSkyBoxShader = nullptr;
	}
	//�O���b�h�����
	if (mDebugGrid)
	{
		delete mDebugGrid;
		mDebugGrid = nullptr;
	}
	// OpenGL�R���e�L�X�g�ƃE�B���h�E
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
	// �e�N�X�`����j�󂷂�
	for (auto i : mTextures)
	{
		i.second->Unload();
		delete i.second;
	}
	mTextures.clear();
	// ���b�V����j�󂷂�
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
	//�t�@�C���p�X�ǉ�
	string filePath = File_P::ModelPath + fileName;
	//�Ԃ������̃��b�V��
	vector<class Mesh*> ms;
	//���b�V���̐����m�F���鏈��
	Mesh* m = nullptr;
	m = new Mesh();
	int maxMesh = m->CheckMeshIndex(filePath, this);
	if (m)
	{
		//���b�V���̉��
		m->Unload();
		delete m;
	}
	for (int i = 0; i < maxMesh; i++)
	{
		string inTex = std::to_string(i);
		Mesh* mesh = nullptr;
		auto iter = mMeshes.find(filePath + inTex.c_str());
		//���łɓǂݍ���ł�����̂Ȃ炻������擾
		if (iter != mMeshes.end())
		{
			mesh = iter->second;
		}
		else
		{
			mesh = new Mesh();
			//������Load�O�Ƀo�C�i���t�@�C�������邩���m�F����
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
	// �쐬�������_�z��ƃC���f�b�N�X�z���VertexArray�쐬

	if (mFanSpriteVerts)
		delete mFanSpriteVerts;
	mFanSpriteVerts = new VertexArray(fillRatio, maxSegments);
	return mFanSpriteVerts->GetNumVerts();
}

void Renderer::CreateAxisVerts()
{
	std::vector<AxisVertex> axisVerts = {
		// X���i�ԁj
		{ Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f) },
		{ Vector3(1.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f) },

		// Y���i�΁j
		{ Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f) },
		{ Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f) },

		// Z���i�j
		{ Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f) },
		{ Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f) },
	};
	mAxisVAO = new VertexArray(axisVerts);
}

void Renderer::SetLightUniforms(Shader* shader, const Matrix4& view)
{
	// �J�����̈ʒu�͋t���܂̎��_����ł�
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
	// screenPoint���f�o�C�X���W�i-1����+1�̊ԁj�ɕϊ�����
	Vector3 deviceCoord = screenPoint;
	deviceCoord.x /= (WindowRenderProperty::GetWidth()) * 0.5f;
	deviceCoord.y /= (WindowRenderProperty::GetHeight()) * 0.5f;

	// �����e�s��Ńx�N�g����ϊ�����
	Matrix4 unprojection = mView * mProjection;
	unprojection.Invert();
	return Vector3::TransformWithPerspDiv(deviceCoord, unprojection);
}

void Renderer::GetScreenDirection(Vector3& outStart, Vector3& outDir) const
{
	// �X�^�[�g�|�C���g���擾����i�߂��̕��ʂ̉�ʂ̒��S�j
	Vector3 screenPoint(0.0f, 0.0f, 0.0f);
	outStart = Unproject(screenPoint);
	// ��ʂ̒����A�߂��Ɖ����̊ԂɃG���h�|�C���g���擾���܂��B
	screenPoint.z = 0.9f;
	Vector3 end = Unproject(screenPoint);
	// �����x�N�g�����擾����
	outDir = end - outStart;
	outDir.Normalize();
}
