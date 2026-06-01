#include "UIActor.h"
#include "ScriptComponent.h"
#include "Component.h"

UIActorObject::UIActorObject(uint64_t id)
	: Entity(id)
	, mIsCanvas(false)
{
	mRectTransform = new RectTransform(this);

	mName = "New UI Actor" + std::to_string(mGame->mNextActorID++);
	mGame->GetUIActorManager()->AddActor(this);
}

UIActorObject::UIActorObject(BaseScene* scene)
	: Entity(scene)
	, mIsCanvas(false)
{
	mRectTransform = new RectTransform(this);
}

UIActorObject::~UIActorObject()
{
	if (mRectTransform) {
		delete mRectTransform;
		mRectTransform = nullptr;
	}
}

void UIActorObject::FixedUpdate(float deltaTime)
{
	if (mState == EActive)
	{
		FixedUpdateComponents(deltaTime);
		mRectTransform->ComputeWorldTransform();
	}
}

void UIActorObject::FixedUpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
	{
		comp->FixedUpdate(deltaTime);
	}
}

void UIActorObject::Update(float deltaTime)
{
	if (mState == EActive)
	{
		UpdateComponents(deltaTime);
		mRectTransform->ComputeWorldTransform();
	}
}

void UIActorObject::EditorComputeWorldTransform()
{
	if (mState == EActive)
	{
		UpdateComponents(Time::gDeltaTime);
		mRectTransform->ComputeWorldTransform();
	}
}

void UIActorObject::Serialize(json& j) const
{
	Entity::Serialize(j);

	j["Type"] = "RecrTransform";
	// ローカルの値を保存する
	j["LocalPosition"] = { mRectTransform->GetLocalPosition().x, mRectTransform->GetLocalPosition().y, mRectTransform->GetLocalPosition().z };
	j["LocalRotation"] = { mRectTransform->GetLocalRotation().w, mRectTransform->GetLocalRotation().x, mRectTransform->GetLocalRotation().y, mRectTransform->GetLocalRotation().z };
	j["LocalScale"] = { mRectTransform->GetLocalScale().x, mRectTransform->GetLocalScale().y, mRectTransform->GetLocalScale().z };

	if (mRectTransform->GetParentActor()) {
		j["ParentActorID"] = mRectTransform->GetParentActor()->GetID();
	}
	else {
		j["ParentActorID"] = -1;
	}

	j["CanvasFrag"] = mIsCanvas;
}

void UIActorObject::Deserialize(const json& j)
{
	Entity::Deserialize(j);

	mRectTransform->SetLocalPosition
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
	mRectTransform->SetLocalRotation(localRotation);
	//GUI上で編集する用キャッシュ数値をVector3で取得
	Vector3 eulerRad = localRotation.ToEulerAngles();
	Vector3 rot;
	rot.x = Math::ToDegrees(eulerRad.x);
	rot.y = Math::ToDegrees(eulerRad.y);
	rot.z = Math::ToDegrees(eulerRad.z);
	mRectTransform->SetRotationEditor(rot);

	mRectTransform->SetLocalScale
	(
		Vector3
		(
			j["LocalScale"][0],
			j["LocalScale"][1],
			j["LocalScale"][2]
		)
	);

	if (j.contains("ParentActorID")) {
		uint64_t id = j.at("ParentActorID").get<uint64_t>();
		mRectTransform->SetParentID(id);
	}

	if (j.contains("CanvasFrag")) {
		mIsCanvas = j.at("CanvasFrag").get<bool>();
	}

	mRectTransform->SetDirty();
}

void UIActorObject::LoadParentByLoadScene()
{
	if (mRectTransform->GetParentID() != -1) {
		mRectTransform->SetParent(mGame->GetUIActorManager()->FindActorByID(mRectTransform->GetParentID()));
	}
}

Entity* UIActorObject::Clone() {
	// 真っ新なアクターを生成
	UIActorObject* clone = new UIActorObject();

	clone->mName = this->mName;
	clone->mState = this->mState;

	clone->GetRectTransform()->SetLocalPosition(this->GetRectTransform()->GetLocalPosition());
	clone->GetRectTransform()->SetLocalRotation(this->GetRectTransform()->GetLocalRotation());
	clone->GetRectTransform()->SetLocalScale(this->GetRectTransform()->GetLocalScale());


	// 4. 自身が持っているコンポーネントのディープコピー
	for (const auto& comp : this->mComponents)
	{
		Component* clonedComp = comp->Clone(clone);
		clone->AddComponent(clonedComp); // 手動でリストに加える
	}
	mGame->SetDirtyFlag(true);
	return clone;
}