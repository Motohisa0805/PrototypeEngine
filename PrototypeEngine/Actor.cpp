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

	// コンポーネントリストのシリアライズ
	nlohmann::json componentsArray = nlohmann::json::array();
	for (const auto& component : mComponents)
	{
		//コンポーネント単体用のjson
		nlohmann::json componentData;
		component->Serialize(componentData); // 各コンポーネントのシリアライズメソッドを呼び出す
		componentsArray.push_back(componentData);
	}
	j["Components"] = componentsArray;
}

void ActorObject::Deserialize(const json& j)
{
	Transform::Deserialize(j);
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
			string type = componentData.at("Type").get<string>();

			// ファクトリーを使ってコンポーネントを生成
			Component* newComponent = ComponentFactory::CreateComponent(type, this);

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

void ActorObject::OnComponentAdded(Component* newComp)
{
	//MeshRendererが追加された場合
	if (MeshRenderer* meshRenderer = dynamic_cast<MeshRenderer*>(newComp))
	{
		//既にBoxColliderがアタッチされているかチェック
		if (BoxCollider* collider = GetComponent<BoxCollider>())
		{
			// BoxColliderのサイズをメッシュに合わせて初期設定する
			// MeshRendererが複数のメッシュを持つ場合は最初のメッシュを使う
			if (!meshRenderer->GetMeshs().empty())
			{
				//BoxColliderにメッシュ境界情報を受け取るSetterが必要

				collider->SetObjectBox(meshRenderer->GetMeshs()[0]->GetBoxs()[0]);
				collider->SetObjectOBB(meshRenderer->GetMeshs()[0]->GetOBBBoxs()[0]);
			}
		}
	}
	//BoxColliderが追加された場合
	else if (BoxCollider* collider = dynamic_cast<BoxCollider*>(newComp))
	{
		//既にMeshRendererがアタッチされているかチェック
		if (MeshRenderer* meshRendeerer = GetComponent<MeshRenderer>())
		{
			//MeshRendererが既にいるので、コライダーのサイズを初期設定する
			if (!meshRenderer->GetMeshs().empty())
			{
				collider->SetObjectBox(meshRenderer->GetMeshs()[0]->GetBoxs()[0]);
				collider->SetObjectOBB(meshRenderer->GetMeshs()[0]->GetOBBBoxs()[0]);
			}
		}
	}
}
