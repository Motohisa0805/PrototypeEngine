#include "Transform.h"
#include "Actor.h" // �ǉ�: ActorObject�̊��S�Ȍ^��񂪕K�v

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
	, mRecomputeWorldTransform(true)
	, mWorldTransform()
	, mModelTransform()
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

	mRecomputeWorldTransform = true;
}

void Transform::ComputeWorldTransform(const Matrix4* parentMatrix)
{
	//�X�V�t���O��true�Ȃ�
	if (mRecomputeWorldTransform)
	{
		mRecomputeWorldTransform = false;

		mModelTransform = Matrix4::CreateScale(mLocalScale);
		mModelTransform *= Matrix4::CreateFromQuaternion(mLocalRotation);
		mModelTransform *= Matrix4::CreateTranslation(mLocalPosition);


		//�e��������
		if (parentMatrix) {
			mWorldTransform = mModelTransform * (*parentMatrix);
		}
		//���Ȃ�������
		else {
			mWorldTransform = mModelTransform;
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
	//�ꎞ�I�ȏC��
	//�q�I�u�W�F�N�g�̍��W�v�Z
	for (auto child : mChildActor)
	{
		child->SetActive();
		child->ComputeWorldTransform(&mWorldTransform);
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
	// ActorObject::OnComponentAdded ���Ăяo��
	// dynamic_cast�̌^�����S�^�ł��邱�Ƃ�ۏ�
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

void Transform::AddChildActor(Transform* actor)
{
	for (Transform* a : mChildActor)
	{
		if (a == actor) { return; }
	}
	// �e�ɂȂ�A�N�^�[�̍s����Čv�Z
	ComputeWorldTransform(mParentActor != nullptr ? &mParentActor->GetWorldTransform() : nullptr);
	Matrix4 parentInvert = mWorldTransform;
	parentInvert.Invert();

	// �q�ɂȂ�A�N�^�[�̍s����Čv�Z
	auto parentActor = actor->GetParentActor();
	actor->ComputeWorldTransform(parentActor != nullptr ? &parentActor->GetWorldTransform() : nullptr);
	Matrix4 child = actor->GetWorldTransform();

	//�e�ɂȂ�A�N�^�[�̋t�s����|���Ďq�̃A�N�^�[�̐e��̃��[�J�������v�Z���Đݒ�
	Matrix4 childLocal = child * parentInvert;
	actor->SetLocalPosition(childLocal.GetTranslation());
	actor->SetLocalRotation(childLocal.GetRotation());
	actor->SetLocalScale(childLocal.GetScale());


	actor->AddParentActor(this);
	mChildActor.push_back(actor);

	//�e�q�֌W�\�z��̍Čv�Z
	mRecomputeWorldTransform = true;
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
	j["Position"] = { mPosition.x, mPosition.y, mPosition.z };
	j["Rotation"] = { mRotation.w, mRotation.x, mRotation.y, mRotation.z };
	j["Scale"] = { mScale.x, mScale.y, mScale.z };
	// �e�q�֌W���V���A���C�Y����ꍇ�͂����ɒǉ�
	//if (mParentActor) { j["Parent"] = mParentActor->GetName(); }
}

void Transform::Deserialize(const json& j)
{
	mPosition.x = j["Position"][0];
	mPosition.y = j["Position"][1];
	mPosition.z = j["Position"][2];

	mRotation.w = j["Rotation"][0];
	mRotation.x = j["Rotation"][1];
	mRotation.y = j["Rotation"][2];
	mRotation.z = j["Rotation"][3];
	
	mScale.x = j["Scale"][0];
	mScale.y = j["Scale"][1];
	mScale.z = j["Scale"][2];
	
	mRecomputeWorldTransform = true;
}
