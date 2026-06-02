#include "EditorSettingsManager.h"
#include "DebugManager.h"
#include "SceneSerializer.h"
#include "ScriptEditManager.h"

const filesystem::path EditorSettingsManager::SETTEINGS_FILE_PATH = "Library/EditorSettings.json";

bool EditorSettingsManager::mIsNoSaveFlag = false;

string EditorSettingsManager::mRenameInputBuffer = "";

bool EditorSettingsManager::mRenaming = false;

filesystem::path EditorSettingsManager::mCurrentFolder = "Assets";

vector<filesystem::path> EditorSettingsManager::mDeleteQueue;

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
			ifs >> mSettings;
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
		ofs << mSettings.dump(2); // インデント2で整形して保存
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

void EditorSettingsManager::CreateNewScene(const filesystem::path& filePath)
{
	string uniqueName = "NewScene.json"; // 拡張子付きで初期化
	filesystem::path targetFolder = mCurrentFolder; // 現在右クリックしているパス（フォルダ）

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
	for (const auto& p : mDeleteQueue)
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
	mDeleteQueue.clear();
}

void EditorSettingsManager::ProcessScriptDelete(const filesystem::path& path)
{
	filesystem::path scriptPath = path;
	if (scriptPath.extension() == ".h")
	{
		filesystem::path cppPath = scriptPath.parent_path() / (scriptPath.stem().string() + ".cpp");
		if (filesystem::exists(cppPath))
		{
			mDeleteQueue.push_back(cppPath);
		}
	}
	else if (scriptPath.extension() == ".cpp")
	{
		filesystem::path hPath = scriptPath.parent_path() / (scriptPath.stem().string() + ".h");
		if (filesystem::exists(hPath))
		{
			mDeleteQueue.push_back(hPath);
		}
	}
}
/*
void EditorSettingsManager::ProcessPendingRenames()
{
	// まずリネームを行う（リネーム後の名前衝突チェックを行う）
	for (const auto& req : mRenameQueue)
	{
		try
		{
			if (!filesystem::exists(req.oldPath))continue;

			//名前前のクラス名と名前変更後のクラス名を特定
			string oldClassName = req.oldPath.stem().string();//例："NewScript"
			string newClassName = req.newStem;                //例："OrignalScript"  

			//C++のスクリプトファイルペアかどうかを判定
			bool isScriptFile = (req.oldPath.extension() == ".h" || req.oldPath.extension() == ".cpp");
			if (req.oldPath.extension().string() == ".tmp_new")
			{
				// 1. ダミーファイルを削除
				if (filesystem::exists(req.oldPath))
				{
					filesystem::remove(req.oldPath);
					Debug::Log("Deleted dummy file: %s\n", req.oldPath.string().c_str());
				}

				// 2. 確定した名前 (req.newStem) でスクリプトファイルを作成
				if (ScriptEditManager::GetInstance().CreateScriptFile(req.oldPath.parent_path(), req.newStem))
				{
					Debug::Log("Created new script: %s\n", req.newStem.c_str());
				}
			}
			// 既存のリネームの場合
			// (oldPathが実在し、スクリプトファイルで、名前が変更されている)
			else if (filesystem::exists(req.oldPath) && isScriptFile && oldClassName != newClassName)
			{
				// ----------------------------------------------------
				// 拡張: ファイルシステム上で、対応する .h と .cpp の両方をリネームする
				// ----------------------------------------------------

				//変更前のパスを計算
				filesystem::path oldHPath = req.oldPath.parent_path() / (oldClassName + ".h");
				filesystem::path oldCppPath = req.oldPath.parent_path() / (oldClassName + ".cpp");

				//変更後のパスを計算
				filesystem::path newHPath = req.oldPath.parent_path() / (newClassName + ".h");
				filesystem::path newCppPath = req.oldPath.parent_path() / (newClassName + ".cpp");

				//1..hファイルのリネーム
				if (filesystem::exists(oldHPath))
				{
					filesystem::rename(oldHPath, newHPath);
					//2..hファイルの内容を書き換え
					ScriptEditManager::GetInstance().ReplaceInFile(newHPath, oldClassName, newClassName);
				}

				//  .cppのロジックのコメントアウトを解除
				//3..cppファイルのリネーム
				if (filesystem::exists(oldCppPath))
				{
					filesystem::rename(oldCppPath, newCppPath);
					//2..cppファイルの内容を書き換え
					ScriptEditManager::GetInstance().ReplaceInFile(newCppPath, oldClassName, newClassName);
				}
			}
			//スクリプト以外 のリネーム
			else if (filesystem::exists(req.oldPath) && !isScriptFile)
			{
				// スクリプトファイルでない、または名前が変わっていない場合は、元のロジックに従い、
				// req.oldPath のファイルのみをリネームキューの定義通りに処理する
				filesystem::path newPath = req.oldPath.parent_path() / (req.newStem + req.oldPath.extension().string());
				if (!filesystem::exists(newPath))
				{
					filesystem::rename(req.oldPath, newPath);
					Debug::Log("Renamed: %s -> %s\\n", req.oldPath.string().c_str(), newPath.string().c_str());
				}
			}
		}
		catch (const exception& e)
		{
			Debug::Log("Rename failed: %s\n", e.what());
		}
	}
	mRenameQueue.clear();
}
*/
