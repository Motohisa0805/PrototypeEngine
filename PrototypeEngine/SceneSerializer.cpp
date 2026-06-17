#include "SceneSerializer.h"
#include "BaseScene.h"
#include "Actor.h"
#include "FreeCamera.h"
#include "DirectionalLightComponent.h"
#include "EditorSettingsManager.h"

filesystem::path SceneSerializer::mTempEditingDirectoryPath = EditorFile::EditorFile_Path;
filesystem::path SceneSerializer::mTempEditingPath = "";

bool SceneSerializer::SaveRunScene(const filesystem::path& filePath, BaseScene* scene)
{
	//JSONオブジェクトの作成
    json saveSceneData;

    //シーンが持つActorリストを取得
    vector<ActorObject*> actors = scene->GetActorManager()->GetActors();
	//Actors配列の作成
	json actorsArray = json::array();
	//各Actorをシリアライズして配列に追加
    for(const auto& actor : actors)
    {
        json actorJson;
        actor->Serialize(actorJson); // ActorObjectのSerializeメソッドを呼び出す
        actorsArray.push_back(actorJson);
	}
	saveSceneData["Actors"] = actorsArray;

    //シーンが持つUIActorリストを取得
    vector<UIActorObject*> uiactors = scene->GetUIActorManager()->GetActors();
    //UIActors配列の作成
    json uiactorsArray = json::array();
    //各UIActorをシリアライズして配列に追加
    for (const auto& actor : uiactors)
    {
        json actorJson;
        actor->Serialize(actorJson); // UIActorObjectのSerializeメソッドを呼び出す
        uiactorsArray.push_back(actorJson);
    }
    saveSceneData["UIActors"] = uiactorsArray;

    WriteEditingSceneData(filePath, scene);

    //ファイル書き込み
    try
    {
        filesystem::path newFilePath = filePath.parent_path() / (filesystem::path)(filePath.stem().string() + ".json");
        std::ofstream ofs(newFilePath);
        if (!ofs.is_open())return false;
        ofs << saveSceneData.dump(2);//2はインデント数(見やすくするため)
        ofs.close();
        return true;
    }
    catch (const std::exception& e)
    {
        Debug::ErrorLog("An exception occurred while exporting the file: %s\n", e.what());
        return false;
    }
}

bool SceneSerializer::CreateEmptyScene(const filesystem::path& filePath)
{
    //JSONオブジェクトの作成
    json createSceneData;

    //Actorsは空の配列として定義
    createSceneData["Actors"] = json::array();
    //UIActorsは空の配列として定義
    createSceneData["UIActors"] = json::array();

    try
    {
        //ファイルを開く
        std::ofstream ofs(filePath);
        if (!ofs.is_open())
        {
            
            return false;
        }

        //JSONデータをファイルに書き込む(インデント：２)
        ofs << createSceneData.dump(2);

        ofs.close();
        return true;
    }
    catch (const std::exception& e)
    {
        Debug::ErrorLog("An exception occurred while loading the file: %s\n", e.what());
        return false;
    }

    return true;
}

BaseScene* SceneSerializer::LoadScene(const string& filePath, bool isWriteTempData)
{
	//1.ファイルからJSONデータを読み込む
	std::ifstream ifs(filePath);
    if (!ifs.is_open())
    {
        return nullptr;
    }

	json loadSceneData;
    try
    {
        ifs >> loadSceneData;
    }
    catch (const std::exception& e)
    {
        ifs.close();
        Debug::ErrorLog("An exception occurred while loading the file: %s\n", e.what());
        return nullptr;
	}

    //2新しいシーンオブジェクトを作成
	EditorScene* newScene = new EditorScene();
    filesystem::path name = filePath;
    newScene->SetName(name.stem().string());
    if (loadSceneData.contains("Actors")) {
        //3.Actorの配列を処理する
        const json& actorsJson = loadSceneData.at("Actors");
        for (const auto& actorData : actorsJson)
        {
            //ActorObjectの新しいインスタンスを作成
		    ActorObject* newActor = new ActorObject(newScene);

            //Deserializeメソッドを呼び出して、JSONデータからActorObjectを初期化
            //この中でActorObject::Deserialize()とComponent::Deserialize()が連鎖して呼び出されます
		    newActor->Deserialize(actorData);

            //シーンにActorを追加
		    newScene->GetActorManager()->AddActor(newActor);
        }
        //各アクターの親子関係構築
        for (const auto& actor : newScene->GetActorManager()->GetActorsMutable())
        {
            actor->LoadParentByLoadScene();
        }
    }
    if (loadSceneData.contains("UIActors")) {
        const json& uiactorsJson = loadSceneData.at("UIActors");
        for (const auto& actorData : uiactorsJson)
        {
            if (actorData.contains("CanvasFrag")) {
                if (actorData.at("CanvasFrag").get<bool>()) {
                    //UIActorObjectの新しいインスタンスを作成
                    Canvas* newCanvas = new Canvas(newScene);

                    //Deserializeメソッドを呼び出して、JSONデータからUIActorObjectを初期化
                    //この中でUIActorObject::Deserialize()とComponent::Deserialize()が連鎖して呼び出されます
                    newCanvas->Deserialize(actorData);

                    //シーンにUIActorを追加
                    newScene->GetUIActorManager()->AddActor(newCanvas);
                }
                else {
                    //UIActorObjectの新しいインスタンスを作成
                    UIActorObject* newActor = new UIActorObject(newScene);

                    //Deserializeメソッドを呼び出して、JSONデータからUIActorObjectを初期化
                    //この中でUIActorObject::Deserialize()とComponent::Deserialize()が連鎖して呼び出されます
                    newActor->Deserialize(actorData);

                    //シーンにUIActorを追加
                    newScene->GetUIActorManager()->AddActor(newActor);
                }
            }
        }
        //各アクターの親子関係構築
        for (const auto& uiactor : newScene->GetUIActorManager()->GetActorsMutable())
        {
            uiactor->LoadParentByLoadScene();
        }
        //一時編集データに書き込みフラグがtrueなら
        if (isWriteTempData) {
            WriteEditingSceneData(filePath,newScene);
        }
    }
    return newScene;
}

void SceneSerializer::RenameRunScene(const filesystem::path& filePath, const string& newFileName)
{
    SceneManager::GetCurrentRunScene()->SetName(newFileName);
    filesystem::path newPath = filePath.parent_path() / (filesystem::path)(newFileName + ".json");
    EditorSettingsManager::GetInstance().SetLastOpenedScene(newPath.string());
}

void SceneSerializer::WriteEditingSceneData(const filesystem::path& filePath, BaseScene* scene)
{
    //現在編集中のJSONオブジェクトの作成
    json editingDataJson;

    //シーンが持つActorリストを取得
    vector<ActorObject*> actors = scene->GetActorManager()->GetActors();
    //Actors配列の作成
    json actorsArray = json::array();
    //各Actorをシリアライズして配列に追加
    for (const auto& actor : actors)
    {
        json actorJson;
        actor->Serialize(actorJson); // ActorObjectのSerializeメソッドを呼び出す
        actorsArray.push_back(actorJson);
    }
    editingDataJson["Actors"] = actorsArray;
    
    //シーンが持つUIActorリストを取得
    vector<UIActorObject*> uiactors = scene->GetUIActorManager()->GetActors();
    //UIActors配列の作成
    json uiactorsArray = json::array();
    //各UIActorをシリアライズして配列に追加
    for (const auto& actor : uiactors)
    {
        json actorJson;
        actor->Serialize(actorJson); // UIActorObjectのSerializeメソッドを呼び出す
        uiactorsArray.push_back(actorJson);
    }
    editingDataJson["UIActors"] = uiactorsArray;
    filesystem::path newEditingPath = filePath.parent_path() / (filesystem::path)(scene->GetName() + ".json");
    //以前の一時ファイルを削除(名前変更にするかは今後検討)
    RelaseEditorData();
    //EditorSettingファイルも編集中のシーン名を変更
    EditorSettingsManager::GetInstance().SetLastOpenedScene(newEditingPath.string());
    // 2. 一時ファイルパスを決定（例：元のファイルパスから一時ファイル名を生成）
    filesystem::path tempPath = mTempEditingDirectoryPath / (filesystem::path)(scene->GetName() + ".json");
    // 3. ファイル書き出しロジックを追加
    try
    {
        std::ofstream ofs(tempPath);
        if (!ofs.is_open()) return;
        ofs << editingDataJson.dump(2);
        ofs.close();
        mTempEditingPath = tempPath;
    }
    catch (const std::exception& e)
    {
        // エラー処理
        Debug::ErrorLog("An exception occurred while exporting the file: %s\n", e.what());
    }
}

void SceneSerializer::RelaseEditorData()
{

    //一時編集ファイルを削除処理
    if (filesystem::exists(mTempEditingPath))
    {
        try
        {
            if (filesystem::remove(mTempEditingPath))
            {
                Debug::Log("Temporary editor file deleted: %s\n", mTempEditingPath.string().c_str());
            }
            else
            {
                Debug::Log("Failed to delete temporary editor file: %s\n", mTempEditingPath.string().c_str());
            }
        }
        catch (const std::exception& e)
        {
            Debug::Log("Exception during temporary file deletion: %s\n", e.what());
        }
    }
}
