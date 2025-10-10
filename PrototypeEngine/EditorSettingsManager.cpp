#include "EditorSettingsManager.h"
#include "DebugManager.h"

const fs::path EditorSettingsManager::SETTEINGS_FILE_PATH = "Library/EditorSettings.json";

EditorSettingsManager& EditorSettingsManager::GetInstance()
{
	static EditorSettingsManager instance;
	return instance;
}

void EditorSettingsManager::LoadSettings()
{
	//Library�t�H���_�����݂��Ȃ���΍쐬
	if (!fs::exists("Library"))
	{
		fs::create_directories("Library");
	}

	//�ݒ�t�@�C�������݂���ꍇ�̂ݓǂݍ���
	if(fs::exists(SETTEINGS_FILE_PATH))
	{
		try
		{
			std::ifstream ifs(SETTEINGS_FILE_PATH);
			ifs >> mSettings;
			Debug::Log("Editor settings loaded from: %s\n", SETTEINGS_FILE_PATH.string().c_str());
		}
		catch (const std::exception& e)
		{
			Debug::Log("Failed to load editor settings: %s\n", e.what());
		}
	}
}

void EditorSettingsManager::SaveSettings()
{
	try
	{
		std::ofstream ofs(SETTEINGS_FILE_PATH);
		ofs << mSettings.dump(2); // �C���f���g2�Ő��`���ĕۑ�
		Debug::Log("Editor settings saved to: %s\n", SETTEINGS_FILE_PATH.string().c_str());
	}
	catch (const std::exception& e)
	{
		Debug::Log("Failed to save editor settings: %s\n", e.what());
	}
}


void EditorSettingsManager::SetLastOpenedScene(const string& path)
{
	mSettings["LastOpenedScene"] = path;
}

string EditorSettingsManager::GetLastOpenedScene() const
{
	//JSON�I�u�W�F�N�g����l���擾�B�L�[�����݂��Ȃ��Ȃ�f�t�H���g�l��Ԃ�
	if(mSettings.count("LastOpenedScene"))
	{
		return mSettings.at("LastOpenedScene").get<string>();
	}
	//�f�t�H���g�̋N���V�[���̃p�X
	return "Assets/Scenes/TestScene01.json";
}
