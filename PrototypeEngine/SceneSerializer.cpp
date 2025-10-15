#include "SceneSerializer.h"
#include "BaseScene.h"
#include "Actor.h"
#include "FreeCamera.h"
#include "DirectionalLightComponent.h"

bool SceneSerializer::SaveScene(const filesystem::path& filePath, BaseScene* scene)
{
    json sceneJson;
    sceneJson["SceneName"] = filePath.stem().string();

    //�V�[��������Actor���X�g���擾
    vector<ActorObject*> actors = scene->GetActors();

	json actorsArray = json::array();

    for(const auto& actor : actors)
    {
        json actorJson;
        actor->Serialize(actorJson); // ActorObject��Serialize���\�b�h���Ăяo��
        actorsArray.push_back(actorJson);
	}

	sceneJson["Actors"] = actorsArray;

    //�t�@�C����������
    try
    {
        std::ofstream ofs(filePath);
        if (!ofs.is_open())return false;
        ofs << sceneJson.dump(2);//2�̓C���f���g��(���₷�����邽��)
        ofs.close();
        return true;
    }
    catch (...)
    {
        return false;
    }

    //return false;
}

bool SceneSerializer::SaveEmptyScene(const filesystem::path& filePath)
{
    //JSON�I�u�W�F�N�g�̍쐬
    json sceneJson;

    //SceneName�ɂ̓t�@�C��������g���q��������������ݒ�
    sceneJson["SceneName"] = filePath.stem().string();

    //Actors�͋�̔z��Ƃ��Ē�`
    sceneJson["Actors"] = json::array();

    try
    {
        //�t�@�C�����J��
        std::ofstream ofs(filePath);
        if (!ofs.is_open())
        {
            
            return false;
        }

        //JSON�f�[�^���t�@�C���ɏ�������(�C���f���g�F�Q)
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
	//1.�t�@�C������JSON�f�[�^��ǂݍ���
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

    //2�V�����V�[���I�u�W�F�N�g���쐬
	EditorScene* newScene = new EditorScene();
    newScene->SetName(StringConverter::ExtractFileName_Fs(filePath));
    //3.Actor�̔z�����������
    const json& actorsJson = sceneJson.at("Actors");
    for (const auto& actorData : actorsJson)
    {
        //ActorObject�̐V�����C���X�^���X���쐬
		ActorObject* newActor = new ActorObject(newScene);

        //Deserialize���\�b�h���Ăяo���āAJSON�f�[�^����ActorObject��������
        //���̒���ActorObject::Deserialize()��Component::Deserialize()���A�����ČĂяo����܂�
		newActor->Deserialize(actorData);

        //�V�[����Actor��ǉ�
		newScene->AddActor(newActor);
    }
    return newScene;
}
