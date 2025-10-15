#pragma once
#include "Typedefs.h"

//�G�f�B�^�̐ݒ���Ǘ�����N���X
//�G���W���̏�Ԃ�JSON�t�@�C���ɕۑ����A����N�����ɓǂݍ���
class EditorSettingsManager
{
private:
	static const filesystem::path			SETTEINGS_FILE_PATH;

	//���݂̃v���W�F�N�g�ݒ��ێ�����JSON�I�u�W�F�N�g
	json							mSettings;
	
	EditorSettingsManager() { LoadSettings(); }
	~EditorSettingsManager() { SaveSettings(); }


	void							LoadSettings();
	void							SaveSettings();
public:
	static EditorSettingsManager&	GetInstance();

	//�ݒ��Getter/Setter
	void							SetLastOpenedScene(const string& path);
	string							GetLastOpenedScene() const;

};

