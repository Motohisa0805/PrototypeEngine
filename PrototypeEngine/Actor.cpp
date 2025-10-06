#include "Actor.h"

ActorObject::ActorObject()
	: Transform()
	, mName("empty")
	, mState(EActive)
	, mActorTag(ActorTag::None)
	, mGame(SceneManager::GetNowScene())
	, mCollider(nullptr)
	, mRigidbody(nullptr)
{
	mGame->AddActor(this);
}

ActorObject::~ActorObject()
{
	mGame->RemoveActor(this);
	// Need to delete components
	// Because ~Component calls RemoveComponent, need a different style loop
	while (!mComponents.empty())
	{
		delete mComponents.back();
	}
	mComponents.clear();
}

void ActorObject::FixedUpdate(float deltaTime)
{
	if (mState == EActive)
	{
		FixedUpdateComponents(deltaTime);
		FixedUpdateActor(deltaTime);
		ComputeWorldTransform(NULL);
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
		ComputeWorldTransform(NULL);
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
	j["Name"] = mName;
	j["State"] = mState;
	j["Tag"] = mActorTag;

	// �R���|�[�l���g���X�g�̃V���A���C�Y
	nlohmann::json componentsArray = nlohmann::json::array();
	for (const auto& component : mComponents)
	{
		nlohmann::json componentData;
		component->Serialize(componentData); // �e�R���|�[�l���g�̃V���A���C�Y���\�b�h���Ăяo��
		componentsArray.push_back(componentData);
	}
	j["Components"] = componentsArray;
}

void ActorObject::Deserialize(const json& j)
{
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
			std::string type = componentData.at("Type").get<std::string>();

			// �t�@�N�g���[���g���ăR���|�[�l���g�𐶐�
			Component* newComponent = new Component(this);

			newComponent = newComponent->CreateComponent(type, this);

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
