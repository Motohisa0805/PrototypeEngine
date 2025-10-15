#include "Transform.h"
#include "Actor.h" // �ǉ�: ActorObject�̊��S�Ȍ^��񂪕K�v

void Transform::AddChild(Transform* child)
{
	//�d���`�F�b�N
	auto iter = std::find(mChildActor.begin(), mChildActor.end(), child);
	if (iter == mChildActor.end())
	{
		mChildActor.push_back(child);
	}
}

void Transform::RemoveChild(Transform* child)
{
	//�d���`�F�b�N
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
	//���[���h���W���烍�[�J�����W���t�v�Z����mLocalPosition���X�V
	mLocalPosition = pos;
	SetDirty();
	ComputeWorldTransform();
}

void Transform::SetRotation(const Quaternion& rotation)
{
	mLocalRotation = rotation;
	SetDirty(); // �X�V�t���O�𗧂Ă�
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
	//�X�V�t���O��false�Ȃ�
	if (!mIsDirty)
	{
		return;
	}


	//���[�J�����W�v�Z
	mLocalTransform = Matrix4::CreateScale(mLocalScale);
	mLocalTransform *= Matrix4::CreateFromQuaternion(mLocalRotation);
	mLocalTransform *= Matrix4::CreateTranslation(mLocalPosition);


	//�e��������A�e�̃��[���h�s����|����
	if (mParentActor)
	{
		// �e�̃��[���h�g�����X�t�H�[�����ŐV�ł��邱�Ƃ�ۏ؂���K�v������
		mParentActor->ComputeWorldTransform();
		mWorldTransform = mLocalTransform * mParentActor->GetWorldTransform();
	}
	//���Ȃ�������
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

void Transform::SetDirty()
{
	if (mIsDirty)
	{
		return;
	}

	//1.�������g���X�V�\�ɂ���
	mIsDirty = true;

	for (auto child : mChildActor)
	{
		child->SetDirty();//�ċA�I�Ƀt���O�𗧂Ă�
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
	// 1. �ύX�s�v�ȃP�[�X�͑������^�[��
	// �����e���Đݒ肵�悤�Ƃ��Ă���
	if (mParentActor == newParent)
	{
		return;
	}
	// �������g��e�ɂ��悤�Ƃ��Ă���
	if (this == newParent)
	{
		return;
	}

	// 2. �֌W����ύX����O�ɁA���݂̃��[���h�g�����X�t�H�[����ێ����Ă���
	//�\�z���O�ɍ��W��ύX���Ă����ꍇ�̏���
	ComputeWorldTransform(); // �ŐV�̏�Ԃ��v�Z
	Matrix4 worldMatrix = GetWorldTransform();

	// 3. ���݂̐e������ꍇ�́A���̐e�̎q���X�g���玩�����폜����
	if (mParentActor)
	{
		mParentActor->RemoveChild(this);
	}

	// 4. �V�����e�q�֌W���\�z����
	mParentActor = newParent;
	if (mParentActor)
	{
		mParentActor->AddChild(this);
	}

	// 5. ���[���h�g�����X�t�H�[�����ێ�����悤�ɁA�V�������[�J���l���v�Z����
	if (mParentActor)
	{
		// �V�����e����ɂ������[�J�����W���t�Z����
		// NewLocal = CurrentWorld * ParentWorld^-1
		mParentActor->ComputeWorldTransform(); // �e�̍s����ŐV��
		Matrix4 parentWorldInverse = mParentActor->GetWorldTransform();
		parentWorldInverse.Invert();

		Matrix4 newLocalMatrix = worldMatrix * parentWorldInverse;
		Matrix4 rotationMatrix = newLocalMatrix.RemoveScale();
		SetLocalPosition(newLocalMatrix.GetTranslation());
		SetLocalRotation(rotationMatrix.GetRotation());
		SetLocalScale(newLocalMatrix.GetScale());
	}
	else // �e���������A���[�g�I�u�W�F�N�g�ɂȂ�ꍇ
	{
		// ���[���h���W�����̂܂܃��[�J�����W�ɂȂ�
		SetLocalPosition(worldMatrix.GetTranslation());
		SetLocalRotation(worldMatrix.GetRotation());
		SetLocalScale(worldMatrix.GetScale());
	}

	// 6. �ύX�����������Ƃ������_�[�e�B�t���O�𗧂Ă�
	SetDirty();
}

void Transform::RemoveParentActor()
{
	SetParent(nullptr);
}

void Transform::Serialize(json& j) const
{
	j["Type"] = "Transform";
	// ���[�J���̒l��ۑ�����
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
