#include "SceneSerializer.h"
#include "BaseScene.h"
#include "Actor.h"
#include "FreeCamera.h"
#include "DirectionalLightComponent.h"
#include "EditorSettingsManager.h"

filesystem::path SceneSerializer::mTempParentPath = EditorFile::EditorFile_Path;
filesystem::path SceneSerializer::mTempPath = "";

bool SceneSerializer::SaveScene(const filesystem::path& filePath, BaseScene* scene)
{
	//JSONオブジェクトの作成
    json sceneJson;
	//SceneNameにはファイル名から拡張子を除いた部分を設定
    sceneJson["SceneName"] = filePath.stem().string();

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
	sceneJson["Actors"] = actorsArray;

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
    sceneJson["UIActors"] = uiactorsArray;

    WriteEditorData(filePath, scene);
    //ファイル書き込み
    try
    {
        std::ofstream ofs(filePath);
        if (!ofs.is_open())return false;
        ofs << sceneJson.dump(2);//2はインデント数(見やすくするため)
        ofs.close();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool SceneSerializer::SaveEmptyScene(const filesystem::path& filePath)
{
    //JSONオブジェクトの作成
    json sceneJson;

    //SceneNameにはファイル名から拡張子を除いた部分を設定
    sceneJson["SceneName"] = filePath.stem().string();

    //Actorsは空の配列として定義
    sceneJson["Actors"] = json::array();
    //UIActorsは空の配列として定義
    sceneJson["UIActors"] = json::array();

    try
    {
        //ファイルを開く
        std::ofstream ofs(filePath);
        if (!ofs.is_open())
        {
            
            return false;
        }

        //JSONデータをファイルに書き込む(インデント：２)
        ofs << sceneJson.dump(2);

        ofs.close();
        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }

    return true;
}

BaseScene* SceneSerializer::LoadScene(const string& filePath)
{
	//1.ファイルからJSONデータを読み込む
	std::ifstream ifs(filePath);
    if (!ifs.is_open())
    {
        return nullptr;
    }

	json sceneJson;
    try
    {
        ifs >> sceneJson;
    }
    catch (const std::exception& e)
    {
        ifs.close();
        return nullptr;
	}

    //2新しいシーンオブジェクトを作成
	EditorScene* newScene = new EditorScene();
    newScene->SetName(sceneJson.at("SceneName").get<std::string>());
    if (sceneJson.contains("Actors")) {
        //3.Actorの配列を処理する
        const json& actorsJson = sceneJson.at("Actors");
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
    }
    if (sceneJson.contains("UIActors")) {
        const json& uiactorsJson = sceneJson.at("UIActors");
        for (const auto& actorData : uiactorsJson)
        {
            //UIActorObjectの新しいインスタンスを作成
            UIActorObject* newActor = new UIActorObject(newScene);

            //Deserializeメソッドを呼び出して、JSONデータからUIActorObjectを初期化
            //この中でUIActorObject::Deserialize()とComponent::Deserialize()が連鎖して呼び出されます
            newActor->Deserialize(actorData);

            //シーンにUIActorを追加
            newScene->GetUIActorManager()->AddActor(newActor);
        }
    }
    WriteEditorData(filePath, newScene);
    return newScene;
}

void SceneSerializer::WriteEditorData(const filesystem::path& filePath, BaseScene* scene)
{
    mTempParentPath = EditorFile::EditorFile_Path;
    //JSONオブジェクトの作成
    json editorDataJson;
    //SceneNameにはファイル名から拡張子を除いた部分を設定
    editorDataJson["SceneName"] = filePath.stem().string();

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
    editorDataJson["Actors"] = actorsArray;
    
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
    editorDataJson["UIActors"] = uiactorsArray;

    // 2. 一時ファイルパスを決定（例：元のファイルパスから一時ファイル名を生成）
    filesystem::path tempPath = mTempParentPath / filePath.filename();

    // 3. ファイル書き出しロジックを追加
    try
    {
        std::ofstream ofs(tempPath);
        if (!ofs.is_open()) return;
        ofs << editorDataJson.dump(2);
        ofs.close();
        mTempPath = tempPath;
    }
    catch (const std::exception& e)
    {
        // エラー処理
    }
}

void SceneSerializer::RelaseEditorData()
{

    //一時編集ファイルを削除処理
    if (filesystem::exists(mTempPath))
    {
        try
        {
            if (filesystem::remove(mTempPath))
            {
                Debug::Log("Temporary editor file deleted: %s\n", mTempPath.string().c_str());
            }
            else
            {
                Debug::Log("Failed to delete temporary editor file: %s\n", mTempPath.string().c_str());
            }
        }
        catch (const std::exception& e)
        {
            Debug::Log("Exception during temporary file deletion: %s\n", e.what());
        }
    }
}
