#include "Transform.h"
#include "SceneManager.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

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

Transform::Transform(ActorObject* owner)
	: Component(owner)
	, mPosition(Vector3::Zero)
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
{
	mName = "Transform";

	mHeaderColor = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
	mHeaderHoveredColor = Vector4(0.3f, 0.6f, 0.3f, 1.0f);
	mHeaderActiveColor = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
}

Transform::~Transform()
{
}

const Transform* Transform::GetChildActor(Transform* actor)
{
	for(Transform* a : mChildActor) {
		if (a == actor) {
			return a;
		}
	}
	return nullptr;
}

void Transform::AddChildActor(Transform* child)
{
	if (child)
	{
		child->SetParent(this);
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
	//Sceneがあるなら
	if (SceneManager::GetNowScene() != nullptr)
	{
		SceneManager::GetNowScene()->SetDirtyFlag(true);
	}
}

void Transform::ActiveDirty()
{
	mIsDirty = true;
}

void Transform::Serialize(json& j) const
{
	Component::Serialize(j);
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

void Transform::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		//Position(Vector3)の編集
		Vector3 pos = mLocalPosition;
		if (ImGui::DragFloat3("Position", &pos.x, 0.1f))//0.1fはドラッグの感度
		{
			//ローカル関数なので注意
			SetLocalPosition(pos);
		}
		//回転だけローカルで取得
		//ローカルならスケール値を含まないため
		Vector3 eulerRad = mLocalRotation.ToEulerAngles();
		Vector3 rot;
		rot.x = Math::ToDegrees(eulerRad.x);
		rot.y = Math::ToDegrees(eulerRad.y);
		rot.z = Math::ToDegrees(eulerRad.z);
		//度数法で表示・編集
		if (ImGui::DragFloat3("Rotation(deg)", &rot.x, 1.0f))
		{

			// ラジアンに変換して保存
			Quaternion qx = Quaternion::CreateFromAxisAngle(Vector3::UnitX, rot.x);
			Quaternion qy = Quaternion::CreateFromAxisAngle(Vector3::UnitY, rot.y);
			Quaternion qz = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, rot.z);
			Quaternion newRotation = qy * qx * qz; // ZYX順で回転を適用
			SetLocalRotation(newRotation);
		}

		//Scale(Vector3)の編集
		Vector3 scale = mLocalScale;
		if (ImGui::DragFloat3("Scale", &scale.x, 0.1f))//0.1fはドラッグの感度
		{
			//ローカル関数なので注意
			SetLocalScale(scale);
		}
	}
}
