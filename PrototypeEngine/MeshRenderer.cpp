#include "MeshRenderer.h"


MeshRenderer::MeshRenderer(ActorObject* owner, bool isSkeletal)
	:Component(owner)
	, mTextureIndex(0)
	, mVisible(true)
	, mIsSkeletal(isSkeletal)
	, mFilePath("")
{
	mName = "MeshRenderer";
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

void MeshRenderer::Serialize(json& j) const
{
	Component::Serialize(j);
	// ���[�h���̃t�@�C���p�X�����̂܂�JSON�ɏ�������
	j["FilePath"] = mFilePath;

	// ���b�V�������_���[�ŗL�̑��̃v���p�e�B���ǉ�
	j["Visible"] = mVisible;
	j["IsSkeletal"] = mIsSkeletal;
}

void MeshRenderer::Deserialize(const json& j)
{
	Component::Deserialize(j);
	//���f���p�X������Ȃ�
	if (j.contains("FilePath"))
	{
		// 1. JSON����t�@�C���p�X���擾����
		std::string filePath = j.at("FilePath").get<std::string>();

		// 2. �����o�ϐ��Ƀt�@�C���p�X��ݒ�
		mFilePath = filePath;

		// 3. �t�@�C���p�X���g���āARenderer���烁�b�V�������[�h���A�ݒ肷��
		//    ���̃R�[�h�ɂ����������������Ŏ��s���܂�
		vector<class Mesh*> mesh = EngineWindow::GetRenderer()->GetMeshs(mFilePath);
		SetMeshs(mesh);
	}

	// 4. ���̑��̃v���p�e�B���ǂݍ���
	mVisible = j.at("Visible").get<bool>();
	mIsSkeletal = j.at("IsSkeletal").get<bool>();
}

void MeshRenderer::DrawGUI()
{
	//MeshRenderer�̃v���p�e�B
	ImGui::Text("Mesh Renderer Properties");

	//1.�t�@�C���p�X�̎擾
	string currentPath = mFilePath;
	static char pathBuffer[256];
	strncpy_s(pathBuffer, currentPath.c_str(), sizeof(pathBuffer));
	pathBuffer[sizeof(pathBuffer) - 1] = '\0';

	//2.�t�@�C���p�X�̓��̓t�B�[���h
	ImGui::InputText("Mesh File Path", pathBuffer, sizeof(pathBuffer), ImGuiInputTextFlags_ReadOnly);

	//3.�t�@�C�����[�h�{�^��(�����Ńt�@�C���I��UI���J�����AProjectPanel�����Drag&Drop��z��)
	//Drag&Drop�z��
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			//�y�C���[�h���t�@�C���p�X�ł���Ɖ���
			const char* dropPath = (const char*)payload->Data;
			string path = StringConverter::ExtensionFileName(dropPath);
			//�t�@�C���p�X���g�����[�h�������Ăяo��
			vector<class Mesh*> mesh = EngineWindow::GetRenderer()->GetMeshs(path);
			SetMeshs(mesh);
		}
		ImGui::EndDragDropTarget();
	}
	/*
	// �{�^���N���b�N�Ńt�@�C���I���_�C�A���O���J������
	if (ImGui::Button("Load Mesh from File"))
	{
		// �O���̃t�@�C���I���_�C�A���O (��: nativefiledialog) ���J���A
		// �I�����ꂽ�t�@�C���p�X�� meshRenderer->Load(...) �ɓn���B
	}
	*/

	ImGui::Separator();
}
