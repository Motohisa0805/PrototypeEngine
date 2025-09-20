#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

namespace fs = std::filesystem;

// �I�𒆂̃t�@�C���p�X��ێ�
static std::string g_SelectedFile;

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
	vector<fs::path> mDeleteQueue; // �폜�\�񃊃X�g

	fs::path mRenameTarget;

	string mRenameBuffer;
	
	bool mRenaming;

	bool mDragDroping;

	bool mShowOverwritePopup = false;
	fs::path mPendingSrc;
	fs::path mPendingDst;

	// �ǉ�:
	vector<RenameRequest> mRenameQueue;

	fs::path mCurrentFolder;
public:
	ProjectPanel(class Renderer* renderer);

	void		Draw(float width, float height);

	// �w�肳�ꂽ�f�B���N�g�����ċA�I�ɕ\��
	void		DrawFolderTree(const fs::path& path);
	void		DrawFileView();

	bool        AssetsFolderPrivateMenu();

	bool		RightClickMenu(const fs::path& path);

	void		DragDropFunction(const fs::path& path);

	void		DrawOverwritePopup();

	void		RenameFunction(const fs::directory_entry entry);

	void		ProcessPendingOperations();

	const char* GetName()override { return "Project"; }
};

