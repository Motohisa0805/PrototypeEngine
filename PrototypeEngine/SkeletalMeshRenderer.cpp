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

				// �s�����x�ɂ���ău�����h�ݒ�i1�񂾂��ōςނȂ烋�[�v�̊O�ł�OK�j
				if (m.Color.w < 1.0f)
				{
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glDepthMask(GL_FALSE);  // �������̂͐[�x�������ݖ����i�C�Ӂj
				}
				else
				{
					glDisable(GL_BLEND);
					glDepthMask(GL_TRUE);   // �s�������̂͒ʏ�ʂ�
				}
				//�}�e���A���̐F��ݒ�
				shader->SetColorUniform("uTexture", m);
				// ���b�V���̒��_�z����A�N�e�B�u�ɐݒ肵�܂�
				VertexArray* va = mMeshs[i]->GetVertexArrays()[j];
				va->SetActive();
				// �`��
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

				// �u�����h�Ȃǂ̓V���h�E�}�b�v�`�掞�͈�ؕs�v
				glDisable(GL_BLEND);
				glDepthMask(GL_TRUE);

				// ���b�V���̒��_�z����A�N�e�B�u�ɐݒ肵�܂�
				VertexArray* va = mMeshs[i]->GetVertexArrays()[j];
				va->SetActive();
				// �`��
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
