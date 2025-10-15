#pragma once
#include "Component.h"
#include "Shader.h"
#include "Actor.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//�O���錾
class Mesh;
//���b�V���̕`����s���R���|�[�l���g�N���X
//�ǂݍ��񂾃��b�V�����Z�b�g���Ďg�p����
//GUI��̏����A�`��A�ǂݍ��ݏ���
class MeshRenderer : public Component
{
protected:
	//���b�V���z��
	vector<Mesh*>				mMeshs;
	//�\������\����
	bool						mVisible;
	//�X�P���^���t���O
	bool						mIsSkeletal;
	//���b�V���t�@�C���p�X
	string 						mFilePath;
	//���b�V���̃A���t�@�l
	float						mAlpha;
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
	//���ɃZ�b�g����Ă��郁�b�V���Ƀv���X�Œǉ�����`�Őݒ�
	virtual void				AddMeshs(const vector<Mesh*>& mesh)
	{
		mMeshs.insert(mMeshs.end(), mesh.begin(), mesh.end());
	}
	//�ǂݍ��񂾕����̃��b�V����ݒ�
	virtual void				SetMeshs(const vector<Mesh*>& mesh)
	{
		mMeshs.clear();
		mMeshs = mesh;
	}

	vector<Mesh*>				GetMeshs() const { return mMeshs; }

	void						SetVisible(bool visible) { mVisible = visible; }
	bool						GetVisible() const { return mVisible; }

	bool						GetIsSkeletal() const { return mIsSkeletal; }
	// �t�@�C���p�X��ݒ肷��Setter��ǉ�
	void						SetMeshFilePath(const std::string& path) { mFilePath = path; }
	const std::string&			GetMeshFilePath() const { return mFilePath; }

	void						SetMaterialAlpha(float alpha);

	void						Serialize(json& j) const override;
	void						Deserialize(const json& j)override;

	void						DrawGUI()override;

};