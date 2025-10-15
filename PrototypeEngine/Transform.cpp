#include "Transform.h"
#include "Actor.h" // 追加: ActorObjectの完全な型情報が必要

void Transform::AddChild(Transform* child)
{
	//重複チェック
	auto iter = std::find(mChildActor.begin(), mChildActor.end(), child);
	if (iter == mChildActor.end())
	{
		mChildActor.push_back(child);
	}
}

void Transform::RemoveChild(Transform* child)
{
	//重複チェック
	auto iter = std::find(mChildActor.begin(), mChildActor.end(), child);
	if (iter != mChildActor.end())
	{
		mChildActor.erase(iter);
	}
}

Transform::Transform()
	: mPosition(Vector3::Zero)
	, mLocalPosition(Vector3::Zero)
	, mPositionOffset(Vector3::Zero)
	, mRotation(Quaternion::Identity)
	, mLocalRotation(Quaternion::Identity)
	, mScale(Vector3(1.0f, 1.0f, 1.0f))
	, mLocalScale(Vector3(1.0f, 1.0f, 1.0f))
	, mIsDirty(true)
	, mWorldTransform()
	, mLocalTransform()
	, mParentActor(nullptr)
	, mChildActor()
	, mComponents()
{
}

Transform::~Transform()
{
}

void Transform::RotateToNewForward(const Vector3& forward)
{
	// Figure out difference between original (unit x) and new
	float dot = Vector3::Dot(Vector3::UnitZ, forward);
	float angle = Math::Acos(dot);
	// Facing down X
	if (dot > 0.9999f)
	{
		SetLocalRotation(Quaternion::Identity);
	}
	// Facing down -X
	else if (dot < -0.9999f)
	{
		SetLocalRotation(Quaternion(Vector3::UnitZ, Math::Pi));
	}
	else
	{
		// Rotate about axis from cross product
		Vector3 axis = Vector3::Cross(Vector3::UnitZ, forward);
		axis.Normalize();
		SetLocalRotation(Quaternion(axis, angle));
	}
}

void Transform::LookAt(const Vector3& targetPosition)
{
	Vector3 currentPosition = mPosition;
	Vector3 forward = (targetPosition - currentPosition).Normalized();
	Vector3 up = Vector3::UnitY;

	Quaternion rot = Quaternion::LookRotation(forward, up);
	SetLocalRotation(rot);

	mIsDirty = true;
}

void Transform::SetPosition(const Vector3& pos)
{
	//ワールド座標からローカル座標を逆計算してmLocalPositionを更新
	mLocalPosition = pos;
	SetDirty();
	ComputeWorldTransform();
}

void Transform::SetRotation(const Quaternion& rotation)
{
	mLocalRotation = rotation;
	SetDirty(); // 更新フラグを立てる
	ComputeWorldTransform();
}

void Transform::SetScale(Vector3 scale)
{
	mLocalScale = scale;
	SetDirty();
	ComputeWorldTransform();
}

void Transform::ComputeWorldTransform()
{
	//更新フラグがfalseなら
	if (!mIsDirty)
	{
		return;
	}


	//ローカル座標計算
	mLocalTransform = Matrix4::CreateScale(mLocalScale);
	mLocalTransform *= Matrix4::CreateFromQuaternion(mLocalRotation);
	mLocalTransform *= Matrix4::CreateTranslation(mLocalPosition);


	//親がいたら、親のワールド行列を掛ける
	if (mParentActor)
	{
		// 親のワールドトランスフォームが最新であることを保証する必要がある
		mParentActor->ComputeWorldTransform();
		mWorldTransform = mLocalTransform * mParentActor->GetWorldTransform();
	}
	//いなかったら
	else
	{
		mWorldTransform = mLocalTransform;
	}
	mPosition = mWorldTransform.GetTranslation();
	mRotation = mWorldTransform.GetRotation();
	mScale = mWorldTransform.GetScale();

	mIsDirty = false;

	// Inform components world transform updated
	for (auto comp : mComponents)
	{
		comp->OnUpdateWorldTransform();
	}
}

const Transform* Transform::GetChildActor(Transform* actor)
{
	for (Transform* a : mChildActor) {
		if (a == actor) {
			return a;
		}
	}
	return nullptr;
}

void Transform::AddComponent(Component* component)
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

void Transform::RemoveComponent(Component* component)
{
	auto iter = std::find(mComponents.begin(), mComponents.end(), component);
	if (iter != mComponents.end())
	{
		mComponents.erase(iter);
	}
}

void Transform::SetDirty()
{
	if (mIsDirty)
	{
		return;
	}

	//1.自分自身を更新可能にする
	mIsDirty = true;

	for (auto child : mChildActor)
	{
		child->SetDirty();//再帰的にフラグを立てる
	}
	SceneManager::GetNowScene()->mIsComputeWorldTransform = true;
}

void Transform::ActiveDirty()
{
	mIsDirty = true;
}

void Transform::AddChildActor(Transform* childactor)
{
	if (childactor)
	{
		childactor->SetParent(this);
	}
}

void Transform::RemoveChildActor(Transform* child)
{
	if (child && child->GetParentActor() == this)
	{
		child->SetParent(nullptr);
	}
}

void Transform::AddParentActor(Transform* parent)
{
	mParentActor = parent;
}

void Transform::SetParent(Transform* newParent)
{
	// 1. 変更不要なケースは早期リターン
	// 同じ親を再設定しようとしている
	if (mParentActor == newParent)
	{
		return;
	}
	// 自分自身を親にしようとしている
	if (this == newParent)
	{
		return;
	}

	// 2. 関係性を変更する前に、現在のワールドトランスフォームを保持しておく
	//構築直前に座標を変更していた場合の処理
	ComputeWorldTransform(); // 最新の状態を計算
	Matrix4 worldMatrix = GetWorldTransform();

	// 3. 現在の親がいる場合は、その親の子リストから自分を削除する
	if (mParentActor)
	{
		mParentActor->RemoveChild(this);
	}

	// 4. 新しい親子関係を構築する
	mParentActor = newParent;
	if (mParentActor)
	{
		mParentActor->AddChild(this);
	}

	// 5. ワールドトランスフォームを維持するように、新しいローカル値を計算する
	if (mParentActor)
	{
		// 新しい親を基準にしたローカル座標を逆算する
		// NewLocal = CurrentWorld * ParentWorld^-1
		mParentActor->ComputeWorldTransform(); // 親の行列を最新に
		Matrix4 parentWorldInverse = mParentActor->GetWorldTransform();
		parentWorldInverse.Invert();

		Matrix4 newLocalMatrix = worldMatrix * parentWorldInverse;
		Matrix4 rotationMatrix = newLocalMatrix.RemoveScale();
		SetLocalPosition(newLocalMatrix.GetTranslation());
		SetLocalRotation(rotationMatrix.GetRotation());
		SetLocalScale(newLocalMatrix.GetScale());
	}
	else // 親を解除し、ルートオブジェクトになる場合
	{
		// ワールド座標がそのままローカル座標になる
		SetLocalPosition(worldMatrix.GetTranslation());
		SetLocalRotation(worldMatrix.GetRotation());
		SetLocalScale(worldMatrix.GetScale());
	}

	// 6. 変更があったことを示すダーティフラグを立てる
	SetDirty();
}

void Transform::RemoveParentActor()
{
	SetParent(nullptr);
}

void Transform::Serialize(json& j) const
{
	j["Type"] = "Transform";
	// ローカルの値を保存する
	j["LocalPosition"] = { mLocalPosition.x, mLocalPosition.y, mLocalPosition.z };
	j["LocalRotation"] = { mLocalRotation.w, mLocalRotation.x, mLocalRotation.y, mLocalRotation.z };
	j["LocalScale"] = { mLocalScale.x, mLocalScale.y, mLocalScale.z };
}

void Transform::Deserialize(const json& j)
{
	mLocalPosition.x = j["LocalPosition"][0];
	mLocalPosition.y = j["LocalPosition"][1];
	mLocalPosition.z = j["LocalPosition"][2];

	mLocalRotation.w = j["LocalRotation"][0];
	mLocalRotation.x = j["LocalRotation"][1];
	mLocalRotation.y = j["LocalRotation"][2];
	mLocalRotation.z = j["LocalRotation"][3];
	
	mLocalScale.x = j["LocalScale"][0];
	mLocalScale.y = j["LocalScale"][1];
	mLocalScale.z = j["LocalScale"][2];
	
	mIsDirty = true;
}
