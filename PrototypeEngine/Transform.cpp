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
		// �e������ꍇ�F
		// ���[���h��]����e�̃��[���h��]�̉e������菜���A���[�J����]���Z�o����
		// L = P^-1 * W  (L: ���[�J��, P: �e�̃��[���h, W: �ݒ肵�������[���h)
		Quaternion inverse = mParentActor->GetRotation();
		Quaternion parentWorldInverse = inverse.Inverse(); // �N�H�[�^�j�I���̋t��]���v�Z
		mLocalRotation = Quaternion::Concatenate(rotation, parentWorldInverse);
	}
	else
	{
		// �e�����Ȃ��ꍇ�A���[���h��] == ���[�J����]
		mLocalRotation = rotation;
	}
	SetDirty(); // �X�V�t���O�𗧂Ă�
}

void Transform::SetScale(Vector3 scale)
{
	if (mParentActor)
	{
		// �e������ꍇ�F
		// ���[���h�X�P�[����e�̃��[���h�X�P�[���̉e���Ŋ���A���[�J���X�P�[�����Z�o
		// �e����]���Ă���ƁA���̌v�Z�͌����ɂ͐������Ȃ����A���p�㑽���̏ꍇ����ŋ@�\
		Vector3 parentScale = mParentActor->GetScale();
		//�[�����Z�������
		if(!Math::NearZero(parentScale.x) && !Math::NearZero(parentScale.y) && !Math::NearZero(parentScale.z))
		{
			mLocalScale = scale / parentScale; // Vector3�̐������Ƃ̏��Z
		}
		else
		{
			// �e�̃X�P�[�����[���̏ꍇ�̃t�H�[���o�b�N
			mLocalScale = Vector3(1.0f, 1.0f, 1.0f);
		}
	}
	else
	{
		//�e�����Ȃ��ꍇ
		mLocalScale = scale;
	}
	SetDirty();
}

void Transform::ComputeWorldTransform()
{
	//�X�V�t���O��true�Ȃ�
	if (mIsDirty)
	{
		mIsDirty = false;

		mLocalTransform = Matrix4::CreateScale(mLocalScale);
		mLocalTransform *= Matrix4::CreateFromQuaternion(mLocalRotation);
		mLocalTransform *= Matrix4::CreateTranslation(mLocalPosition);


		//�e��������
		if (mParentActor)
		{
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
	if (!mIsDirty)
	{
		mIsDirty = true;
		for (auto child : mChildActor)
		{
			child->SetDirty();//�ċA�I�Ƀt���O�𗧂Ă�
		}
	}
}

void Transform::AddChildActor(Transform* actor)
{
	for (Transform* a : mChildActor)
	{
		if (a == actor) { return; }
	}
	// �e�ɂȂ�A�N�^�[�̍s����Čv�Z
	ComputeWorldTransform(/*mParentActor != nullptr ? &mParentActor->GetWorldTransform() : nullptr*/);
	Matrix4 parentInvert = mWorldTransform;
	parentInvert.Invert();

	// �q�ɂȂ�A�N�^�[�̍s����Čv�Z
	auto parentActor = actor->GetParentActor();
	actor->ComputeWorldTransform(/*parentActor != nullptr ? &parentActor->GetWorldTransform() : nullptr*/);
	Matrix4 child = actor->GetWorldTransform();

	//�e�ɂȂ�A�N�^�[�̋t�s����|���Ďq�̃A�N�^�[�̐e��̃��[�J�������v�Z���Đݒ�
	Matrix4 childLocal = child * parentInvert;
	actor->SetLocalPosition(childLocal.GetTranslation());
	actor->SetLocalRotation(childLocal.GetRotation());
	actor->SetLocalScale(childLocal.GetScale());


	actor->AddParentActor(this);
	mChildActor.push_back(actor);

	//�e�q�֌W�\�z��̍Čv�Z
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
