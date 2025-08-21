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

void MeshRenderer::DrawForShadowMap(Shader* shader)
{
	for (unsigned int i = 0; i < mMeshs.size(); i++)
	{
		for (unsigned int j = 0; j < mMeshs[i]->GetVertexArrays().size(); j++)
		{
			if (!mMeshs[i]) continue;

			// ���[���h�ϊ��̂ݐݒ�
			shader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());

			// �u�����h�Ȃǂ̓V���h�E�}�b�v�`�掞�͈�ؕs�v
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);

			// ���_�z����A�N�e�B�u��
			VertexArray* va = mMeshs[i]->GetVertexArrays()[j];
			va->SetActive();

			// �`��
			glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
		}
	}
}
