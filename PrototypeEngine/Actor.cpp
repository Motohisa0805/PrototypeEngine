#include "Actor.h"
#include "ScriptComponent.h"
#include "Component.h"

ActorObject::ActorObject(uint64_t id)
	: Entity(id)
{
	mTransform = new Transform(this);

	mName = "Actor" + std::to_string(mGame->mNextActorID++);
	mGame->GetActorManager()->AddActor(this);
}

ActorObject::ActorObject(BaseScene* scene)
	: Entity(scene)
{
	//Transformの生成
	mTransform = new Transform(this);
}

ActorObject::~ActorObject()
{
	/*
	if (mGame && mGame->GetActorManager())
	{
		mGame->GetActorManager()->RemoveActor(this);
	}
	*/

	// TransformはActorObjectが所有しているので、ここで削除
	if (mTransform)
	{
		delete mTransform;
		mTransform = nullptr;
	}
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

/*
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

void ActorObject::OnEnabled()
{
	for (auto comp : mComponents)
	{
		comp->SetIsRun(true);
	}
}

void ActorObject::OnDisable() 
{
	for (auto comp : mComponents)
	{
		comp->SetIsRun(false);
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
*/

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
	Entity::Serialize(j);

	j["Type"] = "Transform";
	// ローカルの値を保存する
	j["LocalPosition"] = { mTransform->GetLocalPosition().x, mTransform->GetLocalPosition().y, mTransform->GetLocalPosition().z };
	j["LocalRotation"] = { mTransform->GetLocalRotation().w, mTransform->GetLocalRotation().x, mTransform->GetLocalRotation().y, mTransform->GetLocalRotation().z };
	j["LocalScale"] = { mTransform->GetLocalScale().x, mTransform->GetLocalScale().y, mTransform->GetLocalScale().z };
}

void ActorObject::Deserialize(const json& j)
{
	Entity::Deserialize(j);

	mTransform->SetLocalPosition
	(
		Vector3
		(
			j["LocalPosition"][0],
			j["LocalPosition"][1],
			j["LocalPosition"][2]
		)
	);
	Quaternion localRotation = Quaternion
	(
		j["LocalRotation"][1],
		j["LocalRotation"][2],
		j["LocalRotation"][3],
		j["LocalRotation"][0]
	);
	mTransform->SetLocalRotation(localRotation);
	//GUI上で編集する用キャッシュ数値をVector3で取得
	Vector3 eulerRad = localRotation.ToEulerAngles();
	Vector3 rot;
	rot.x = Math::ToDegrees(eulerRad.x);
	rot.y = Math::ToDegrees(eulerRad.y);
	rot.z = Math::ToDegrees(eulerRad.z);
	mTransform->SetRotationEditor(rot);

	mTransform->SetLocalScale
	(
		Vector3
		(
			j["LocalScale"][0],
			j["LocalScale"][1],
			j["LocalScale"][2]
		)
	);
	mTransform->SetDirty();
}

ActorObject* ActorObject::Clone()
{
	// 真っ新なアクターを生成
	ActorObject* clone = new ActorObject();

	clone->mName = this->mName;
	clone->mState = this->mState;

	clone->GetTransform()->SetLocalPosition(this->GetTransform()->GetLocalPosition());
	clone->GetTransform()->SetLocalRotation(this->GetTransform()->GetLocalRotation());
	clone->GetTransform()->SetLocalScale(this->GetTransform()->GetLocalScale());


	// 4. 自身が持っているコンポーネントのディープコピー
	for (const auto& comp : this->mComponents)
	{
		Component* clonedComp = comp->Clone(clone);
		clone->AddComponent(clonedComp); // 手動でリストに加える
	}
	mGame->SetDirtyFlag(true);
	return clone;
}
