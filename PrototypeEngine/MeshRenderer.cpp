#include "MeshRenderer.h"


MeshRenderer::MeshRenderer(ActorObject* owner, bool isSkeletal)
	:Component(owner)
	, mVisible(true)
	, mIsSkeletal(isSkeletal)
	, mFilePath("")
	, mAlpha(1.0f)
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

void MeshRenderer::SetMaterialAlpha(float alpha)
{
	float a = Math::Clamp(alpha, 0.0f, 1.0f);
	if (!mMeshs.empty())
	{
		for (auto& mesh : mMeshs)
		{
			vector<MaterialInfo> info = mesh->GetMaterialInfo();
			for (int i = 0; i < info.size(); ++i)
			{
				info[i].Color = Vector4(info[i].Color.x, info[i].Color.y, info[i].Color.z, a);
			}
			mesh->SetMaterialInfo(info);
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
	j["Alpha"] = mAlpha;
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
		AddMeshs(mesh);
	}

	// 4. ���̑��̃v���p�e�B���ǂݍ���
	if (j.contains("Visible"))
	{
		mVisible = j.at("Visible").get<bool>();
	}
	if (j.contains("IsSkeletal"))
	{
		mIsSkeletal = j.at("IsSkeletal").get<bool>();
	}
	if (j.contains("Alpha"))
	{
		mAlpha = j.at("Alpha").get<float>();
		SetMaterialAlpha(mAlpha);
	}
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
	ImGui::NewLine();
	ImGui::Text("FilePath DragDropTarget");
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
			mAlpha = mesh[0]->GetMaterialInfo()[0].Color.w;
			mFilePath = path;
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::NewLine();

	if (!mMeshs.empty())
	{
		ImGui::Text("Alpha Setting");
		ImGui::SliderFloat("Alpha", &mAlpha, 0.0f, 1.0f,"%.2f");
		ImGui::SetNextItemWidth(50);
		ImGui::DragFloat("Alpha Input", &mAlpha, 1.0f, 0.0f, 1.0f, "%.2f");

		if (mAlpha != mMeshs[0]->GetMaterialInfo()[0].Color.w)
		{
			SetMaterialAlpha(mAlpha);
		}
	}

	ImGui::Separator();
}
