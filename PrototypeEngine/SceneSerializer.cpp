#include "SceneSerializer.h"
#include "BaseScene.h"
#include "Actor.h"
#include "FreeCamera.h"
#include "DirectionalLightComponent.h"

bool SceneSerializer::SaveScene(const fs::path& filePath, BaseScene* scene)
{
    json sceneJson;
    sceneJson["SceneName"] = filePath.stem().string();

    //シーンが持つActorリストを取得
    vector<ActorObject*> actors = scene->GetActors();

	json actorsArray = json::array();

    for(const auto& actor : actors)
    {
        json actorJson;
        actor->Serialize(actorJson); // ActorObjectのSerializeメソッドを呼び出す
        actorsArray.push_back(actorJson);
	}

	sceneJson["Actors"] = actorsArray;

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

    //return false;
}

bool SceneSerializer::SaveEmptyScene(const fs::path& filePath)
{
    //JSONオブジェクトの作成
    json sceneJson;

    //SceneNameにはファイル名から拡張子を除いた部分を設定
    sceneJson["SceneName"] = filePath.stem().string();

    //Actorsは空の配列として定義
    sceneJson["Actors"] = json::array();

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
    newScene->SetName(StringConverter::ExtractFileName_Fs(filePath));
    //3.Actorの配列を処理する
    const json& actorsJson = sceneJson.at("Actors");
    for (const auto& actorData : actorsJson)
    {
        //ActorObjectの新しいインスタンスを作成
		ActorObject* newActor = new ActorObject(/* scene: newScene */);

        //Deserializeメソッドを呼び出して、JSONデータからActorObjectを初期化
        //この中でActorObject::Deserialize()とComponent::Deserialize()が連鎖して呼び出されます
		newActor->Deserialize(actorData);

        //シーンにActorを追加
		newScene->AddActor(newActor);
    }

    /*
    //ここでシーン内のオブジェクトは全て生成されている。
    //もし読み込んだシーンにカメラ、環境光がないなら生成
    FreeCamera* mainCam = nullptr;
    DirectionalLightComponent* dirLight = nullptr;
    //シーンにあるか調べる
    for (auto* actor : newScene->GetActors())
    {
        if (!mainCam)
        {
            mainCam = actor->GetComponent<FreeCamera>();
        }
        if (!dirLight)
        {
            dirLight = actor->GetComponent<DirectionalLightComponent>();
        }
        //2つともあったらfor文を抜ける
        if (mainCam && dirLight)break;
    }

    if (!mainCam)
    {
        ActorObject* cameraActor = new ActorObject();
        cameraActor->SetName("MainCamera");
        //初期位置と回転を設定
        cameraActor->SetPosition(Vector3(0.0f, 2.0f, -5.0f));
        cameraActor->SetRotation(Quaternion(Vector3::UnitY, 0.0f));
        
        //TODO : ここはカメラコンポーネントを改造して変更予定
        //FreeCameraコンポーネントをアタッチ
        FreeCamera* freeCamComp = new FreeCamera(cameraActor);
        freeCamComp->SetIsMain(true);//メインカメラに設定
    }

    //LightActorがないなら
    if (!dirLight)
    {
        ActorObject* lightActor = new ActorObject();
        lightActor->SetName("Directional Light");
        // 太陽光のデフォルト回転 (例: X軸で-45度回転、Y軸で45度回転)
        // 90度：X→Yに向く → Y成分 = 1（昼！）
        Quaternion rot = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, -45.0f); 
        lightActor->SetLocalRotation(rot);

        // DirectionalLightComponent をアタッチ
        new DirectionalLightComponent(lightActor);
    }
    */
    return newScene;
}
