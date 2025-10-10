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
	//Libraryフォルダが存在しなければ作成
	if (!fs::exists("Library"))
	{
		fs::create_directories("Library");
	}

	//設定ファイルが存在する場合のみ読み込む
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
		ofs << mSettings.dump(2); // インデント2で整形して保存
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
	//JSONオブジェクトから値を取得。キーが存在しないならデフォルト値を返す
	if(mSettings.count("LastOpenedScene"))
	{
		return mSettings.at("LastOpenedScene").get<string>();
	}
	//デフォルトの起動シーンのパス
	return "Assets/Scenes/TestScene01.json";
}
