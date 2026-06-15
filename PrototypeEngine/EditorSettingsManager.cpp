#include "EditorSettingsManager.h"
#include "DebugManager.h"
#include "SceneSerializer.h"
#include "ScriptEditManager.h"

const filesystem::path EditorSettingsManager::SETTEINGS_FILE_PATH = "Library/EditorSettings.json";

bool EditorSettingsManager::mIsNoSaveFlag = false;

string EditorSettingsManager::mRenameInputBuffer = "";

bool EditorSettingsManager::mIsRenaming = false;

vector<filesystem::path> EditorSettingsManager::mDeleteDirectoryQueue;

EditorSettingsManager& EditorSettingsManager::GetInstance()
{
	static EditorSettingsManager instance;
	return instance;
}

void EditorSettingsManager::LoadSettings()
{
	//Libraryフォルダが存在しなければ作成
	if (!filesystem::exists("Library"))
	{
		filesystem::create_directories("Library");
	}

	//設定ファイルが存在する場合のみ読み込む
	if(filesystem::exists(SETTEINGS_FILE_PATH))
	{
		try
		{
			std::ifstream ifs(SETTEINGS_FILE_PATH);
			ifs >> mEditorSettingData;
			Debug::Log("Editor settings loaded from: %s\n", SETTEINGS_FILE_PATH.string().c_str());
		}
		catch (const std::exception& e)
		{
			Debug::Log("Failed to load editor settings: %s\n", e.what());
		}
	}
}

void EditorSettingsManager::SaveEditorSettings()
{
	try
	{
		std::ofstream ofs(SETTEINGS_FILE_PATH);
		ofs << mEditorSettingData.dump(2); // インデント2で整形して保存
		ofs.close();
		Debug::Log("Editor settings saved to: %s\n", SETTEINGS_FILE_PATH.string().c_str());
	}
	catch (const std::exception& e)
	{
		Debug::Log("Failed to save editor settings: %s\n", e.what());
		return;
	}
}


void EditorSettingsManager::SetLastOpenedScene(const string& path)
{
	mEditorSettingData["LastOpenedScene"] = path;
	SaveEditorSettings();
}

string EditorSettingsManager::GetLastOpenedScene() const
{
	//JSONオブジェクトから値を取得。キーが存在しないならデフォルト値を返す
	if(mEditorSettingData.count("LastOpenedScene"))
	{
		return mEditorSettingData.at("LastOpenedScene").get<string>();
	}
	//デフォルトの起動シーンのパス
	return "Assets/Scenes/TestScene01.json";
}

void EditorSettingsManager::CreateNewScene(const filesystem::path& filePath, const filesystem::path& selectingPath)
{
	string uniqueName = "NewScene.json"; // 拡張子付きで初期化
	filesystem::path targetFolder = selectingPath; // 現在右クリックしているパス（フォルダ）

	// 既に存在するファイル名かチェックし、ユニークな名前に変更する
	int counter = 1;
	while (filesystem::exists(targetFolder / uniqueName)) {
		// NewScene(1).json, NewScene(2).json のように生成
		uniqueName = "NewScene (" + std::to_string(counter++) + ").json";
	}

	filesystem::path newScenePath = targetFolder / uniqueName;

	// 3. SceneSerializerを使って空のシーンデータをファイルに書き出す
	// SceneSerializer::SaveEmptyScene()内でファイル書き込み処理を行う
	if (SceneSerializer::SaveEmptyScene(newScenePath))
	{
		// 成功ログ
		Debug::Log("Created new scene: %s\n", newScenePath.string().c_str());
	}
	else
	{
		// 失敗ログ
		Debug::Log("Failed to create scene file: %s\n", newScenePath.string().c_str());
	}
}

void EditorSettingsManager::ProcessPendingDeletions()
{
	// 削除処理
	for (const auto& p : mDeleteDirectoryQueue)
	{
		try
		{
			if (!filesystem::exists(p)) continue;

			// 2. ファイルシステムからの削除
			if (filesystem::is_directory(p))
			{
				// フォルダの場合、配下のすべてのファイルを削除
				filesystem::remove_all(p);
				Debug::Log("Deleted folder: %s\n", p.string().c_str());
			}
			else // ファイルの場合
			{
				filesystem::remove(p);
				Debug::Log("Deleted file: %s\n", p.string().c_str());
			}
		}
		catch (const exception& e)
		{
			Debug::Log("Delete failed: %s\n", e.what());
		}
	}
	mDeleteDirectoryQueue.clear();
}

void EditorSettingsManager::ProcessScriptDelete(const filesystem::path& path)
{
	filesystem::path scriptPath = path;
	if (scriptPath.extension() == ".h")
	{
		filesystem::path cppPath = scriptPath.parent_path() / (scriptPath.stem().string() + ".cpp");
		if (filesystem::exists(cppPath))
		{
			mDeleteDirectoryQueue.push_back(cppPath);
		}
	}
	else if (scriptPath.extension() == ".cpp")
	{
		filesystem::path hPath = scriptPath.parent_path() / (scriptPath.stem().string() + ".h");
		if (filesystem::exists(hPath))
		{
			mDeleteDirectoryQueue.push_back(hPath);
		}
	}
}
