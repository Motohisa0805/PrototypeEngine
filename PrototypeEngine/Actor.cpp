#include "Actor.h"

#include "MeshRenderer.h"
#include "BoxCollider.h"

ActorObject::ActorObject()
	: Transform()
	, mGame(SceneManager::GetNowScene())
	, mName("Actor")
	, mState(EActive)
	, mActorTag(ActorTag::None)
	, mCollider(nullptr)
	, mRigidbody(nullptr)
{
	mName = "Actor" + std::to_string(mGame->mNextActorID++);
	mGame->AddActor(this);
}

ActorObject::ActorObject(BaseScene* scene)
	: Transform()
	, mGame(scene)
	, mName("Actor")
	, mState(EActive)
	, mActorTag(ActorTag::None)
	, mCollider(nullptr)
	, mRigidbody(nullptr)
{
}

ActorObject::~ActorObject()
{
	mGame->RemoveActor(this);
	// Need to delete components
	// Because ~Component calls RemoveComponent, need a different style loop
	for (int i = 0; i < mComponents.size(); i++)
	{
		delete mComponents[i];
	}
	mComponents.clear();
}

void ActorObject::FixedUpdate(float deltaTime)
{
	if (mState == EActive)
	{
		FixedUpdateComponents(deltaTime);
		FixedUpdateActor(deltaTime);
		ComputeWorldTransform();
	}
}

void ActorObject::FixedUpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
	{
		comp->FixedUpdate(deltaTime);
	}
}

void ActorObject::FixedUpdateActor(float deltaTime)
{
}

void ActorObject::Update(float deltaTime)
{
	if (mState == EActive)
	{
		//ComputeLocalTransform();
		UpdateComponents(deltaTime);
		UpdateActor(deltaTime);
		ComputeWorldTransform();
	}
}

void ActorObject::UpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
	{
		comp->Update(deltaTime);
	}
}

void ActorObject::UpdateActor(float deltaTime)
{
}

void ActorObject::ProcessInput(const struct InputState& keyState)
{
	if (mState == EActive)
	{
		// First process input for components
		for (auto comp : mComponents)
		{
			comp->ProcessInput(keyState);
		}

		ActorInput(keyState);
	}
}

void ActorObject::ActorInput(const struct InputState& keyState)
{
}

void ActorObject::Serialize(json& j) const
{
	Transform::Serialize(j);
	j["Name"] = mName;
	j["State"] = mState;
	j["Tag"] = mActorTag;

	// �R���|�[�l���g���X�g�̃V���A���C�Y
	nlohmann::json componentsArray = nlohmann::json::array();
	for (const auto& component : mComponents)
	{
		//�R���|�[�l���g�P�̗p��json
		nlohmann::json componentData;
		component->Serialize(componentData); // �e�R���|�[�l���g�̃V���A���C�Y���\�b�h���Ăяo��
		componentsArray.push_back(componentData);
	}
	j["Components"] = componentsArray;
}

void ActorObject::Deserialize(const json& j)
{
	Transform::Deserialize(j);
	// ���O��ǂݍ���
	mName = j.at("Name").get<std::string>();

	// ��Ԃ�ǂݍ���
	mState = static_cast<State>(j.at("State").get<int>());

	// �^�O��ǂݍ���
	mActorTag = static_cast<ActorTag>(j.at("Tag").get<int>());

	// �R���|�[�l���g���X�g������
	if (j.contains("Components"))
	{
		for (const auto& componentData : j.at("Components"))
		{
			// �R���|�[�l���g�̌^�����
			string type = componentData.at("Type").get<string>();

			// �t�@�N�g���[���g���ăR���|�[�l���g�𐶐�
			Component* newComponent = ComponentFactory::CreateComponent(type, this);

			if (newComponent)
			{
				// ���������R���|�[�l���g��JSON�f�[�^��n���A������
				newComponent->Deserialize(componentData);
				// ActorObject�ɃR���|�[�l���g���A�^�b�`
				AddComponent(newComponent);
			}
		}
	}
}

void ActorObject::OnComponentAdded(Component* newComp)
{
	//MeshRenderer���ǉ����ꂽ�ꍇ
	if (MeshRenderer* meshRenderer = dynamic_cast<MeshRenderer*>(newComp))
	{
		//����BoxCollider���A�^�b�`����Ă��邩�`�F�b�N
		if (BoxCollider* collider = GetComponent<BoxCollider>())
		{
			// BoxCollider�̃T�C�Y�����b�V���ɍ��킹�ď����ݒ肷��
			// MeshRenderer�������̃��b�V�������ꍇ�͍ŏ��̃��b�V�����g��
			if (!meshRenderer->GetMeshs().empty())
			{
				//BoxCollider�Ƀ��b�V�����E�����󂯎��Setter���K�v

				collider->SetObjectBox(meshRenderer->GetMeshs()[0]->GetBoxs()[0]);
				collider->SetObjectOBB(meshRenderer->GetMeshs()[0]->GetOBBBoxs()[0]);
			}
		}
	}
	//BoxCollider���ǉ����ꂽ�ꍇ
	else if (BoxCollider* collider = dynamic_cast<BoxCollider*>(newComp))
	{
		//����MeshRenderer���A�^�b�`����Ă��邩�`�F�b�N
		if (MeshRenderer* meshRendeerer = GetComponent<MeshRenderer>())
		{
			//MeshRenderer�����ɂ���̂ŁA�R���C�_�[�̃T�C�Y�������ݒ肷��
			if (!meshRenderer->GetMeshs().empty())
			{
				collider->SetObjectBox(meshRenderer->GetMeshs()[0]->GetBoxs()[0]);
				collider->SetObjectOBB(meshRenderer->GetMeshs()[0]->GetOBBBoxs()[0]);
			}
		}
	}
}
