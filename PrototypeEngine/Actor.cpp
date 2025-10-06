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

	// コンポーネントリストのシリアライズ
	nlohmann::json componentsArray = nlohmann::json::array();
	for (const auto& component : mComponents)
	{
		nlohmann::json componentData;
		component->Serialize(componentData); // 各コンポーネントのシリアライズメソッドを呼び出す
		componentsArray.push_back(componentData);
	}
	j["Components"] = componentsArray;
}

void ActorObject::Deserialize(const json& j)
{
	// 名前を読み込む
	mName = j.at("Name").get<std::string>();

	// 状態を読み込む
	mState = static_cast<State>(j.at("State").get<int>());

	// タグを読み込む
	mActorTag = static_cast<ActorTag>(j.at("Tag").get<int>());

	// コンポーネントリストを処理
	if (j.contains("Components"))
	{
		for (const auto& componentData : j.at("Components"))
		{
			// コンポーネントの型を特定
			std::string type = componentData.at("Type").get<std::string>();

			// ファクトリーを使ってコンポーネントを生成
			Component* newComponent = new Component(this);

			newComponent = newComponent->CreateComponent(type, this);

			if (newComponent)
			{
				// 生成したコンポーネントにJSONデータを渡し、初期化
				newComponent->Deserialize(componentData);
				// ActorObjectにコンポーネントをアタッチ
				AddComponent(newComponent);
			}
		}
	}
}
