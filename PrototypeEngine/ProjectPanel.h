#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"
#include "SceneSerializer.h"

//�v���W�F�N�g�̃t�@�C���p�X��`�悷��N���X
//�{�i�I�ȕ`�揈���͖�����
class ProjectPanel : public GUIPanel
{
public:
	struct RenameRequest 
	{
		fs::path oldPath;
		std::string newStem; // �g���q�� oldPath.extension() ���g���čČ�������
	};
private:
	vector<fs::path>		mDeleteQueue; // �폜�\�񃊃X�g

	fs::path				mRenameTarget;

	string					mRenameBuffer;
	
	bool					mRenaming;

	bool					mDragDroping;

	bool					mShowOverwritePopup = false;
	// �ۗ����̑���
	fs::path				mPendingSrc;
	fs::path				mPendingDst;
	// �ǉ�:
	vector<RenameRequest>	mRenameQueue;
	fs::path				mCurrentFolder;
	fs::path				mCurrentFile;
	// ���[�U�[�����N���b�N�Ńn�C���C�g�����t�@�C��/�t�H���_
	fs::path				mSelectedPath;  
public:
	//�I�𒆂̃t�@�C���p�X���擾
	const char* GetName()override { return "Project"; }
	//�R���X�g���N�^
	ProjectPanel(class Renderer* renderer);
	//GUI�̏�����
	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;
	//GUI�̕`��
	void		Draw(float width, float height, ImTextureRef ref = nullptr)override;
	// Assets�t�H���_�̉E�N���b�N���j���[
	bool        AssetsFolderPrivateMenu();
	// �w�肳�ꂽ�f�B���N�g�����ċA�I�ɕ\��
	void		DrawFolderTree(const fs::path& path);
	void		DrawFileView();
	// 1�̃t�@�C��/�t�H���_��`��
	void		DrawFileSystemEntry(const fs::directory_entry& entry);
	// �E�N���b�N���j���[
	bool		RightClickMenu(const fs::path& path);
	//�V���[�g�J�b�g�L�[����
	void        ShortcutKeyInputFunction(const fs::path& path);
	//�h���b�O���h���b�v
	void		DragDropFunction(const fs::path& path);
	//���l�[������
	void		RenameFunction(const fs::directory_entry entry);
	//�㏑���m�F�|�b�v�A�b�v
	void		DrawOverwritePopup();
	//�ۗ����̍폜�A���l�[���A�h���b�O���h���b�v�̏���
	void		ProcessPendingOperations();
};

