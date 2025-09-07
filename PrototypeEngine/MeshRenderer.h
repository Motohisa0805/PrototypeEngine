#pragma once
#include "Component.h"
#include "Shader.h"
#include "Actor.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//�O���錾
class Mesh;
//���b�V���̕`����s���N���X
//�ǂݍ��񂾃��b�V�����Z�b�g���Ďg�p����
class MeshRenderer : public Component
{
protected:
	vector<Mesh*>				mMeshs;
	size_t						mTextureIndex;
	bool						mVisible;
	bool						mIsSkeletal;
public:
								MeshRenderer(ActorObject* owner, bool isSkeletal = false);
								~MeshRenderer();
	// ���̃��b�V���R���|�[�l���g��`��
	virtual void				Draw(Shader* shader);
	virtual void				DrawForShadowMap(Shader* shader);
	// ���b�V���R���|�[�l���g�Ŏg�p����郁�b�V��/�e�N�X�`���C���f�b�N�X��ݒ肷��
	virtual void				SetMesh(Mesh* mesh) 
	{
		mMeshs.push_back(mesh);
	}

	virtual void				SetMeshs(const vector<Mesh*>& mesh)
	{
		mMeshs.insert(mMeshs.end(), mesh.begin(), mesh.end());
	}

	void						SetTextureIndex(size_t index) { mTextureIndex = index; }

	void						SetVisible(bool visible) { mVisible = visible; }
	bool						GetVisible() const { return mVisible; }

	bool						GetIsSkeletal() const { return mIsSkeletal; }

	vector<Mesh*>				GetMeshs() const { return mMeshs; }

    void SetMaterialAlpha(float alpha)
	{
		float a = Math::Clamp(alpha, 0.0f, 1.0f);
		if (!mMeshs.empty())
		{
			for (auto& mesh : mMeshs)
			{
				vector<MaterialInfo> info = mesh->GetMaterialInfo();
				for(int i = 0; i < info.size(); ++i)
				{
					info[i].Color = Vector4(info[i].Color.x, info[i].Color.y, info[i].Color.z,a);
				}
				mesh->SetMaterialInfo(info);
			}
		}
	}
};