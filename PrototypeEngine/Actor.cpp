#include "Actor.h"
#include "Component.h"
#include "ScriptComponent.h"

ActorObject::ActorObject(uint64_t id) : Entity(id)
{
    mTransform = new Transform(this);

    mName = "New Actor" + std::to_string(mGame->mNextActorID++);
    mGame->GetActorManager()->AddActor(this);
}

ActorObject::ActorObject(BaseScene* scene) : Entity(scene)
{
    // Transformの生成
    mTransform = new Transform(this);
}

ActorObject::~ActorObject()
{
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

void ActorObject::OnCollisionEnter(ActorObject* target)
{
    for (auto comp : mComponents)
    {
        ScriptComponent* scriptComp = dynamic_cast<ScriptComponent*>(comp);
        // 1.ScriptComponentであるか確認
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
        // 1.ScriptComponentであるか確認
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
        // 1.ScriptComponentであるか確認
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
    j["LocalPosition"] = {mTransform->GetLocalPosition().x,
                          mTransform->GetLocalPosition().y,
                          mTransform->GetLocalPosition().z};
    j["LocalRotation"] = {
        mTransform->GetLocalRotation().w, mTransform->GetLocalRotation().x,
        mTransform->GetLocalRotation().y, mTransform->GetLocalRotation().z};
    j["LocalScale"] = {mTransform->GetLocalScale().x,
                       mTransform->GetLocalScale().y,
                       mTransform->GetLocalScale().z};

    if (mTransform->GetParentActor())
    {
        j["ParentActorID"] = mTransform->GetParentActor()->GetID();
    }
    else
    {
        j["ParentActorID"] = -1;
    }
}

void ActorObject::Deserialize(const json& j)
{
    Entity::Deserialize(j);

    mTransform->SetLocalPosition(Vector3(
        j["LocalPosition"][0], j["LocalPosition"][1], j["LocalPosition"][2]));
    Quaternion localRotation =
        Quaternion(j["LocalRotation"][1], j["LocalRotation"][2],
                   j["LocalRotation"][3], j["LocalRotation"][0]);
    mTransform->SetLocalRotation(localRotation);
    // GUI上で編集する用キャッシュ数値をVector3で取得
    Vector3 eulerRad = localRotation.ToEulerAngles();
    Vector3 rot;
    rot.x = Math::ToDegrees(eulerRad.x);
    rot.y = Math::ToDegrees(eulerRad.y);
    rot.z = Math::ToDegrees(eulerRad.z);
    mTransform->SetRotationEditor(rot);

    mTransform->SetLocalScale(
        Vector3(j["LocalScale"][0], j["LocalScale"][1], j["LocalScale"][2]));

    if (j.contains("ParentActorID"))
    {
        uint64_t id = j.at("ParentActorID").get<uint64_t>();
        mTransform->SetParentID(id);
    }

    mTransform->SetDirty();
}

void ActorObject::LoadParentByLoadScene()
{
    if (mTransform->GetParentID() != -1)
    {
        mTransform->SetParent(
            mGame->GetActorManager()->FindActorByID(mTransform->GetParentID()));
    }
}

Entity* ActorObject::Clone()
{
    // 真っ新なアクターを生成
    ActorObject* clone = new ActorObject();

    clone->mName  = this->mName;
    clone->mState = this->mState;

    clone->GetTransform()->SetLocalPosition(
        this->GetTransform()->GetLocalPosition());
    clone->GetTransform()->SetLocalRotation(
        this->GetTransform()->GetLocalRotation());
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
