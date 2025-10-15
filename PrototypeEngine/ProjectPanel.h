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
		filesystem::path	oldPath;
		// �g���q�� oldPath.extension() ���g���čČ�������
		string				newStem; 
	};
private:
	vector<filesystem::path>		mDeleteQueue; // �폜�\�񃊃X�g

	filesystem::path				mPathToRename;

	string							mRenameInputBuffer;
	
	bool							mRenaming;

	bool							mShowOverwritePopup = false;
	// �ۗ����̑���
	filesystem::path				mPendingSrc;
	filesystem::path				mPendingDst;
	// �ǉ�:
	vector<RenameRequest>			mRenameQueue;
	filesystem::path				mCurrentFolder;
	filesystem::path				mCurrentFile;
	// ���[�U�[�����N���b�N�Ńn�C���C�g�����t�@�C��/�t�H���_
	filesystem::path				mSelectedPath;  
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
	void		DrawFolderTree(const filesystem::path& path);
	void		DrawFileView();
	// 1�̃t�@�C��/�t�H���_��`��
	void		DrawFileSystemEntry(const filesystem::directory_entry& entry);
	// �E�N���b�N���j���[
	bool		RightClickMenu(const filesystem::path& path);
	//�V���[�g�J�b�g�L�[����
	void        ShortcutKeyInputFunction(const filesystem::path& path);
	//�h���b�O���h���b�v
	void		DragDropFunction(const filesystem::path& path);
	//���l�[������
	void		RenameFunction(const filesystem::directory_entry entry);
	//�㏑���m�F�|�b�v�A�b�v
	void		DrawOverwritePopup();
	//�ۗ����̍폜�A���l�[���A�h���b�O���h���b�v�̏���
	void		ProcessPendingOperations();
};

