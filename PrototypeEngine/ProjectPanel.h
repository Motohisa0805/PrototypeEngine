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
private:
public:
	ProjectPanel(class Renderer* renderer);

	void		Draw(float width, float height);
	// �w�肳�ꂽ�f�B���N�g�����ċA�I�ɕ\��
	void		ShowDirectoryRecursive(const fs::path& path);

	const char* GetName()override { return "Project"; }
};

