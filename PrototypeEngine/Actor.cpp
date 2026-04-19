#include "Actor.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "SceneManager.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "ComponentFactory.h"
#include "ScriptComponent.h"

#include "Component.h"

ActorObject::ActorObject()
	: mComponents()
	, mGame(SceneManager::GetNowScene())
	, mName("Actor")
	, mState(EActive)
	, mActorTag(ActorInformation::Tag::None)
	, mCollider(nullptr)
	, mRigidbody(nullptr)
{
	mTransform = new Transform(this);

	mName = "Actor" + std::to_string(mGame->mNextActorID++);
	mGame->GetActorManager()->AddActor(this);
}

ActorObject::ActorObject(BaseScene* scene)
	: mGame(scene)
	, mName("Actor")
	, mState(EActive)
	, mActorTag(ActorInformation::Tag::None)
	, mCollider(nullptr)
	, mRigidbody(nullptr)
{
	//Transformの生成
	mTransform = new Transform(this);
}

ActorObject::~ActorObject()
{
	if (mGame && mGame->GetActorManager())
	{
		mGame->GetActorManager()->RemoveActor(this);
	}

	// TransformはActorObjectが所有しているので、ここで削除
	if (mTransform)
	{
		delete mTransform;
		mTransform = nullptr;
	}
	// Need to delete components
	// Because ~Component calls RemoveComponent, need a different style loop
	for (auto comp : mComponents)
	{
		delete comp;
	}
	mComponents.clear();
}

void ActorObject::FixedUpdate(float deltaTime)
{
	if (mState == EActive)
	{
		FixedUpdateComponents(deltaTime);
		mTransform->ComputeWorldTransform();
	}
}

void ActorObject::FixedUpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
	{
		comp->FixedUpdate(deltaTime);
	}
}

void ActorObject::Update(float deltaTime)
{
	if (mState == EActive)
	{
		//ComputeLocalTransform();
		UpdateComponents(deltaTime);
		mTransform->ComputeWorldTransform();
	}
}

void ActorObject::EditorComputeWorldTransform()
{
	if (mState == EActive)
	{
		UpdateComponents(Time::gDeltaTime);
		mTransform->ComputeWorldTransform();
	}
}

void ActorObject::UpdateComponents(float deltaTime)
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

void ActorObject::StateUpdate(float deltaTime)
{
	//状態が変化していない場合は何もしない
	if(mState == mPreviousState)
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

void ActorObject::OnDestroy()
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

void ActorObject::AddComponent(Component* component)
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
}

void ActorObject::RemoveComponent(Component* component)
{
	auto iter = std::find(mComponents.begin(), mComponents.end(), component);
	if (iter != mComponents.end())
	{
		mComponents.erase(iter);
	}
}

void ActorObject::OnCollisionEnter(ActorObject* target)
{
	for (auto comp : mComponents)
	{
		ScriptComponent* scriptComp = dynamic_cast<ScriptComponent*>(comp);
		//1.ScriptComponentであるか確認
		if (scriptComp != nullptr)
		{
			scriptComp->OnCollisionEnter(target);
		}
	}
}

void ActorObject::OnCollisionStay(ActorObject* target)
{
	for (auto comp : mComponents)
	{
		ScriptComponent* scriptComp = dynamic_cast<ScriptComponent*>(comp);
		//1.ScriptComponentであるか確認
		if (scriptComp != nullptr)
		{
			scriptComp->OnCollisionStay(target);
		}
	}
}

void ActorObject::OnCollisionExit(ActorObject* target)
{
	for (auto comp : mComponents)
	{
		ScriptComponent* scriptComp = dynamic_cast<ScriptComponent*>(comp);
		//1.ScriptComponentであるか確認
		if (scriptComp != nullptr)
		{
			scriptComp->OnCollisionExit(target);
		}
	}
}

void ActorObject::Serialize(json& j) const
{
	j["Type"] = "Transform";
	// ローカルの値を保存する
	j["LocalPosition"] = { mTransform->GetLocalPosition().x, mTransform->GetLocalPosition().y, mTransform->GetLocalPosition().z };
	j["LocalRotation"] = { mTransform->GetLocalRotation().w, mTransform->GetLocalRotation().x, mTransform->GetLocalRotation().y, mTransform->GetLocalRotation().z };
	j["LocalScale"] = { mTransform->GetLocalScale().x, mTransform->GetLocalScale().y, mTransform->GetLocalScale().z };


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
	mTransform->SetLocalPosition
	(
		Vector3
		(
			j["LocalPosition"][0],
			j["LocalPosition"][1],
			j["LocalPosition"][2]
		)
	);

	mTransform->SetLocalRotation
	(
		Quaternion
		(
			j["LocalRotation"][1],
			j["LocalRotation"][2],
			j["LocalRotation"][3],
			j["LocalRotation"][0]
		)
	);

	mTransform->SetLocalScale
	(
		Vector3
		(
			j["LocalScale"][0],
			j["LocalScale"][1],
			j["LocalScale"][2]
		)
	);

	//mTransform->ActiveDirty();

	// 名前を読み込む
	mName = j.at("Name").get<std::string>();

	// 状態を読み込む
	mState = static_cast<State>(j.at("State").get<int>());

	// タグを読み込む
	mActorTag = static_cast<ActorInformation::Tag>(j.at("Tag").get<int>());

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
				mTransform->SetDirty();
			}
		}
	}
}

void ActorObject::OnComponentAdded(Component* newComp)
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
