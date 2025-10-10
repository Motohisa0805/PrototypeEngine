#include "SceneSerializer.h"
#include "BaseScene.h"
#include "Actor.h"
#include "FreeCamera.h"
#include "DirectionalLightComponent.h"

bool SceneSerializer::SaveScene(const fs::path& filePath, BaseScene* scene)
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

bool SceneSerializer::SaveEmptyScene(const fs::path& filePath)
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
		ActorObject* newActor = new ActorObject(/* scene: newScene */);

        //Deserialize���\�b�h���Ăяo���āAJSON�f�[�^����ActorObject��������
        //���̒���ActorObject::Deserialize()��Component::Deserialize()���A�����ČĂяo����܂�
		newActor->Deserialize(actorData);

        //�V�[����Actor��ǉ�
		newScene->AddActor(newActor);
    }

    /*
    //�����ŃV�[�����̃I�u�W�F�N�g�͑S�Đ�������Ă���B
    //�����ǂݍ��񂾃V�[���ɃJ�����A�������Ȃ��Ȃ琶��
    FreeCamera* mainCam = nullptr;
    DirectionalLightComponent* dirLight = nullptr;
    //�V�[���ɂ��邩���ׂ�
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
        //2�Ƃ���������for���𔲂���
        if (mainCam && dirLight)break;
    }

    if (!mainCam)
    {
        ActorObject* cameraActor = new ActorObject();
        cameraActor->SetName("MainCamera");
        //�����ʒu�Ɖ�]��ݒ�
        cameraActor->SetPosition(Vector3(0.0f, 2.0f, -5.0f));
        cameraActor->SetRotation(Quaternion(Vector3::UnitY, 0.0f));
        
        //TODO : �����̓J�����R���|�[�l���g���������ĕύX�\��
        //FreeCamera�R���|�[�l���g���A�^�b�`
        FreeCamera* freeCamComp = new FreeCamera(cameraActor);
        freeCamComp->SetIsMain(true);//���C���J�����ɐݒ�
    }

    //LightActor���Ȃ��Ȃ�
    if (!dirLight)
    {
        ActorObject* lightActor = new ActorObject();
        lightActor->SetName("Directional Light");
        // ���z���̃f�t�H���g��] (��: X����-45�x��]�AY����45�x��])
        // 90�x�FX��Y�Ɍ��� �� Y���� = 1�i���I�j
        Quaternion rot = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, -45.0f); 
        lightActor->SetLocalRotation(rot);

        // DirectionalLightComponent ���A�^�b�`
        new DirectionalLightComponent(lightActor);
    }
    */
    return newScene;
}
