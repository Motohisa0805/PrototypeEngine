#include "Entity.h"
#include "MeshRenderer.h"
#include "SceneManager.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "ComponentFactory.h"
#include "ScriptComponent.h"
#include "Component.h"
#include "UIActor.h"

static uint64_t sNextEntityID = 1;

Entity::Entity(uint64_t id)
	: mID(0)
	, mComponents()
	, mGame(SceneManager::GetCurrentRunScene())
	, mName("Entity")
	, mState(EActive)
	, mActorTag(ActorInformation::Tag::None)
	, mPreviousState(State::EActive)
{
	if (id == 0) {
		// 新規生成なら新しい番号を振る
		mID = sNextEntityID++;
	}
	else {
		// ロード時は元のIDを引き継ぐ
		mID = id;
		if (id >= sNextEntityID)sNextEntityID = id + 1;
	}

}

Entity::Entity(BaseScene* scene)
	: mID(0)
	, mGame(scene)
	, mName("Entity")
	, mState(EActive)
	, mActorTag(ActorInformation::Tag::None)
	, mPreviousState(State::EActive)
{
}

Entity::~Entity()
{
	for (auto comp : mComponents)
	{
		delete comp;
	}
	mComponents.clear();
}

void Entity::FixedUpdate(float deltaTime)
{
	if (mState == EActive)
	{
		FixedUpdateComponents(deltaTime);
	}
}

void Entity::FixedUpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
	{
		comp->FixedUpdate(deltaTime);
	}
}

void Entity::Update(float deltaTime)
{
	if (mState == EActive)
	{
		//ComputeLocalTransform();
		UpdateComponents(deltaTime);
	}
}

void Entity::EditorComputeWorldTransform()
{
	if (mState == EActive)
	{
		UpdateComponents(Time::gDeltaTime);
	}
}

void Entity::UpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
	{
		ScriptComponent* scriptComp = dynamic_cast<ScriptComponent*>(comp);
		//1.ScriptComponentであるか確認
		if (scriptComp != nullptr)
		{
			//2.Start()が呼ばれたか確認
			if (!scriptComp->HasStarted())
			{
				scriptComp->Awake();
				scriptComp->OnEnable();
				scriptComp->Start();
				scriptComp->SetStarted(true);
			}
		}
		comp->Update(deltaTime);
	}
}

void Entity::StateUpdate(float deltaTime)
{
	//状態が変化していない場合は何もしない
	if (mState == mPreviousState)
	{
		return;
	}
	//状態が変化したときに、ScriptComponentのOnEnable/OnDisableを呼び出す
	if (mState != mPreviousState)
	{
		for (auto comp : mComponents)
		{
			ScriptComponent* scriptComp = dynamic_cast<ScriptComponent*>(comp);
			//1.ScriptComponentであるか確認
			if (scriptComp != nullptr)
			{
				if (mState == EPaused || mState == EDead)
				{
					scriptComp->OnDisable();
				}
				else if (mState == EActive)
				{
					scriptComp->OnEnable();
				}
			}
		}
	}
	//最後に状態を更新
	mPreviousState = mState;
}

void Entity::ProcessInput(const struct InputState& keyState)
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

void Entity::ActorInput(const struct InputState& keyState)
{
}

void Entity::OnDestroy()
{
	for (auto comp : mComponents)
	{
		ScriptComponent* scriptComp = dynamic_cast<ScriptComponent*>(comp);
		//1.ScriptComponentであるか確認
		if (scriptComp != nullptr)
		{
			scriptComp->OnDisable();
			scriptComp->Destroy();
		}
	}
}

void Entity::OnEnabled()
{
	for (auto comp : mComponents)
	{
		comp->SetIsRun(true);
	}
}

void Entity::OnDisable()
{
	for (auto comp : mComponents)
	{
		comp->SetIsRun(false);
	}
}

void Entity::AddComponent(Component* component)
{
	// Find the insertion point in the sorted vector
	int myOrder = component->GetUpdateOrder();
	auto iter = mComponents.begin();
	for (; iter != mComponents.end(); ++iter)
	{
		if (myOrder < (*iter)->GetUpdateOrder())
		{
			break;
		}
	}

	mComponents.insert(iter, component);
	// ActorObject::OnComponentAdded を呼び出す
	// dynamic_castの型が完全型であることを保証
	if (ActorObject* actor = dynamic_cast<ActorObject*>(this))
	{
		actor->OnComponentAdded(component);
	}
	else if (UIActorObject* uiactor = dynamic_cast<UIActorObject*>(this)) {
		uiactor->OnComponentAdded(component);
	}
}

void Entity::RemoveComponent(Component* component)
{
	auto iter = std::find(mComponents.begin(), mComponents.end(), component);
	if (iter != mComponents.end())
	{
		mComponents.erase(iter);
	}
}


void Entity::Serialize(json& j) const
{
	j["ID"] = mID;
	j["Name"] = mName;
	j["State"] = mState;
	j["Tag"] = mActorTag;
	j["Static"] = mStatic;

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

void Entity::Deserialize(const json& j)
{
	//IDをロードする時
	if (j.contains("ID")) {
		uint64_t id = j.at("ID").get<uint64_t>();
		mID = id;
		if (id >= sNextEntityID)sNextEntityID = id + 1;
	}
	//IDがないなら
	else {
		mID = sNextEntityID++;
	}

	if (j.contains("Name")) {
		// 名前を読み込む
		mName = j.at("Name").get<std::string>();
	}

	if (j.contains("State")) {
		// 状態を読み込む
		mState = static_cast<State>(j.at("State").get<int>());
	}

	if (j.contains("Tag")) {
		// タグを読み込む
		mActorTag = static_cast<ActorInformation::Tag>(j.at("Tag").get<int>());
	}

	if (j.contains("Static")) {
		// 静的状態タグを読み込む
		mStatic = static_cast<ActorInformation::StaticTag>(j.at("Static").get<uint32_t>());
	}

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

void Entity::OnComponentAdded(Component* newComp)
{
	MeshRenderer* meshRenderer = nullptr;
	//MeshRendererが追加された場合
	if (meshRenderer = dynamic_cast<MeshRenderer*>(newComp))
	{
		//既にBoxColliderがアタッチされているかチェック
		if (BoxCollider* collider = GetComponent<BoxCollider>())
		{
			// BoxColliderのサイズをメッシュに合わせて初期設定する
			// MeshRendererが複数のメッシュを持つ場合は最初のメッシュを使う
			if (!meshRenderer->GetMeshs().empty())
			{
				//BoxColliderにメッシュ境界情報を受け取るSetterが必要

				collider->SetObjectAABB(meshRenderer->GetMeshs()[0]->GetBoxs()[0]);
				collider->SetObjectOBB(meshRenderer->GetMeshs()[0]->GetOBBBoxs()[0]);
			}
		}
	}
	//BoxColliderが追加された場合
	else if (BoxCollider* collider = dynamic_cast<BoxCollider*>(newComp))
	{
		//既にMeshRendererがアタッチされているかチェック
		meshRenderer = GetComponent<MeshRenderer>();
		if (meshRenderer != nullptr)
		{
			//MeshRendererが既にいるので、コライダーのサイズを初期設定する
			if (!meshRenderer->GetMeshs().empty())
			{
				collider->SetObjectAABB(meshRenderer->GetMeshs()[0]->GetBoxs()[0]);
				collider->SetObjectOBB(meshRenderer->GetMeshs()[0]->GetOBBBoxs()[0]);
			}
		}
	}
	else if (SphereCollider* sphere = dynamic_cast<SphereCollider*>(newComp))
	{
		//既にMeshRendererがアタッチされているかチェック
		meshRenderer = GetComponent<MeshRenderer>();
		if (meshRenderer != nullptr)
		{
			//MeshRendererが既にいるので、コライダーのサイズを初期設定する
			if (!meshRenderer->GetMeshs().empty())
			{
				sphere->SetObjectSphere(meshRenderer->GetMeshs()[0]->GetAABBFromSphere());
			}
		}
	}
	else if (CapsuleCollider* capsule = dynamic_cast<CapsuleCollider*>(newComp))
	{
		//既にMeshRendererがアタッチされているかチェック
		meshRenderer = GetComponent<MeshRenderer>();
		if (meshRenderer != nullptr)
		{
			//MeshRendererが既にいるので、コライダーのサイズを初期設定する
			if (!meshRenderer->GetMeshs().empty())
			{
				capsule->SetObjectCapsule(meshRenderer->GetMeshs()[0]->GetAABBFromCapsule());
			}
		}
	}
}