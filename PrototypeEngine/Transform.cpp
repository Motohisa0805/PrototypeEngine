#include "Transform.h"
#include "Actor.h" // 追加: ActorObjectの完全な型情報が必要

Transform::Transform()
	: mPosition(Vector3::Zero)
	, mLocalPosition(Vector3::Zero)
	, mPositionOffset(Vector3::Zero)
	, mRotation(Quaternion::Identity)
	, mLocalRotation(Quaternion::Identity)
	, mRotationAmountX(0)
	, mRotationAmountY(0)
	, mRotationAmountZ(0)
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
	if (mParentActor)
	{
		Matrix4 parentWorldInverse = mParentActor->GetWorldTransform();
		parentWorldInverse.Invert();
		mLocalPosition = Vector3::Transform(pos, parentWorldInverse);
	}
	else
	{
		mLocalPosition = pos;
	}
	SetDirty();
}

void Transform::SetRotation(const Quaternion& rotation)
{
	if (mParentActor)
	{
		// 親がいる場合：
		// ワールド回転から親のワールド回転の影響を取り除き、ローカル回転を算出する
		// L = P^-1 * W  (L: ローカル, P: 親のワールド, W: 設定したいワールド)
		Quaternion inverse = mParentActor->GetRotation();
		Quaternion parentWorldInverse = inverse.Inverse(); // クォータニオンの逆回転を計算
		mLocalRotation = Quaternion::Concatenate(rotation, parentWorldInverse);
	}
	else
	{
		// 親がいない場合、ワールド回転 == ローカル回転
		mLocalRotation = rotation;
	}
	SetDirty(); // 更新フラグを立てる
}

void Transform::SetScale(Vector3 scale)
{
	if (mParentActor)
	{
		// 親がいる場合：
		// ワールドスケールを親のワールドスケールの影響で割り、ローカルスケールを算出
		// 親が回転していると、この計算は厳密には正しくないが、実用上多くの場合これで機能
		Vector3 parentScale = mParentActor->GetScale();
		//ゼロ除算を避ける
		if(!Math::NearZero(parentScale.x) && !Math::NearZero(parentScale.y) && !Math::NearZero(parentScale.z))
		{
			mLocalScale = scale / parentScale; // Vector3の成分ごとの除算
		}
		else
		{
			// 親のスケールがゼロの場合のフォールバック
			mLocalScale = Vector3(1.0f, 1.0f, 1.0f);
		}
	}
	else
	{
		//親がいない場合
		mLocalScale = scale;
	}
	SetDirty();
}

void Transform::ComputeWorldTransform()
{
	//更新フラグがtrueなら
	if (mIsDirty)
	{
		mIsDirty = false;

		mLocalTransform = Matrix4::CreateScale(mLocalScale);
		mLocalTransform *= Matrix4::CreateFromQuaternion(mLocalRotation);
		mLocalTransform *= Matrix4::CreateTranslation(mLocalPosition);


		//親がいたら
		if (mParentActor)
		{
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

		// Inform components world transform updated
		for (auto comp : mComponents)
		{
			comp->OnUpdateWorldTransform();
		}
	}
	//一時的な修正
	//子オブジェクトの座標計算
	for (auto child : mChildActor)
	{
		child->SetDirty();
	}
}

void Transform::LocalBonePositionUpdateActor(Matrix4 boneMatrix, const Matrix4& parentActor)
{
	Vector3 position = parentActor.GetTranslation() + boneMatrix.GetTranslation();
	position += mPositionOffset;
	SetLocalPosition(position);
	Quaternion r = Quaternion(boneMatrix.GetRotation());
	r = Quaternion::Concatenate(r, Quaternion(Vector3::UnitX, mRotationAmountX));
	r = Quaternion::Concatenate(r, Quaternion(Vector3::UnitY, mRotationAmountY));
	r = Quaternion::Concatenate(r, Quaternion(Vector3::UnitZ, mRotationAmountZ));
	SetLocalRotation(r);
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
	if (!mIsDirty)
	{
		mIsDirty = true;
		for (auto child : mChildActor)
		{
			child->SetDirty();//再帰的にフラグを立てる
		}
	}
}

void Transform::AddChildActor(Transform* actor)
{
	for (Transform* a : mChildActor)
	{
		if (a == actor) { return; }
	}
	// 親になるアクターの行列を再計算
	ComputeWorldTransform(/*mParentActor != nullptr ? &mParentActor->GetWorldTransform() : nullptr*/);
	Matrix4 parentInvert = mWorldTransform;
	parentInvert.Invert();

	// 子になるアクターの行列を再計算
	auto parentActor = actor->GetParentActor();
	actor->ComputeWorldTransform(/*parentActor != nullptr ? &parentActor->GetWorldTransform() : nullptr*/);
	Matrix4 child = actor->GetWorldTransform();

	//親になるアクターの逆行列を掛けて子のアクターの親基準のローカル情報を計算して設定
	Matrix4 childLocal = child * parentInvert;
	actor->SetLocalPosition(childLocal.GetTranslation());
	actor->SetLocalRotation(childLocal.GetRotation());
	actor->SetLocalScale(childLocal.GetScale());


	actor->AddParentActor(this);
	mChildActor.push_back(actor);

	//親子関係構築後の再計算
	mIsDirty = true;
}

void Transform::RemoveChildActor(Transform* child)
{
	auto iter = std::find(mChildActor.begin(), mChildActor.end(), child);
	if (iter != mChildActor.end())
	{
		child->AddParentActor(nullptr);
		mChildActor.erase(iter);
	}
}

void Transform::AddParentActor(Transform* parent)
{
	mParentActor = parent;
}

void Transform::RemoveParentActor()
{
	mParentActor = nullptr;
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
