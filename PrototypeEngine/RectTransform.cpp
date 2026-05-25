#include "RectTransform.h"
#include "SceneManager.h"
#include "UIActor.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

void RectTransform::AddChild(UIActorObject* child)
{
	//重複チェック
	auto iter = std::find(mChildActor.begin(), mChildActor.end(), child);
	if (iter == mChildActor.end())
	{
		mChildActor.push_back(child);
	}
}

void RectTransform::RemoveChild(UIActorObject* child)
{
	//重複チェック
	auto iter = std::find(mChildActor.begin(), mChildActor.end(), child);
	if (iter != mChildActor.end())
	{
		mChildActor.erase(iter);
	}
}

RectTransform::RectTransform(UIActorObject* owner)
	:Component(owner)
	, mPosition(Vector3::Zero)
	, mLocalPosition(Vector3::Zero)
	, mPositionOffset(Vector3::Zero)
	, mRotation(Quaternion::Identity)
	, mLocalRotation(Quaternion::Identity)
	, mScale(Vector3(1.0f,1.0f, 1.0f))
	, mLocalScale(Vector3(1.0f,1.0f, 1.0f))
	, mIsDirty(true)
	, mWorldTransform()
	, mLocalTransform()
	, mParentActor(nullptr)
	, mChildActor()
{
	mName = "RectTransform";

	mHeaderColor = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
	mHeaderHoveredColor = Vector4(0.3f, 0.6f, 0.3f, 1.0f);
	mHeaderActiveColor = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
}

RectTransform::~RectTransform()
{
}

void RectTransform::SetPosition(const Vector3& pos)
{
	//ワールド座標からローカル座標を逆計算してmLocalPositionを更新
	mLocalPosition = pos;
	SetDirty();
	ComputeWorldTransform();
}

void RectTransform::SetRotation(const Quaternion& rotation)
{
	mLocalRotation = rotation;
	SetDirty(); // 更新フラグを立てる
	ComputeWorldTransform();
}

void RectTransform::SetScale(Vector3 scale)
{
	mLocalScale = scale;
	SetDirty();
	ComputeWorldTransform();
}

void RectTransform::ComputeWorldTransform() {

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
		mParentActor->GetRectTransform()->ComputeWorldTransform();
		mWorldTransform = mLocalTransform * mParentActor->GetRectTransform()->GetWorldTransform();
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
	for (auto comp : mOwner->GetComponents())
	{
		comp->OnUpdateWorldTransform();
	}
}

const UIActorObject* RectTransform::GetChildActor(UIActorObject* actor)
{
	for (UIActorObject* a : mChildActor) {
		if (a == actor) {
			return a;
		}
	}
	return nullptr;
}

void RectTransform::AddChildActor(UIActorObject* childtransform)
{
	if (childtransform)
	{
		childtransform->GetRectTransform()->SetParent(mUIActor);
	}
}

void RectTransform::RemoveChildActor(UIActorObject* childtransform)
{
	if (childtransform && childtransform->GetRectTransform()->GetParentActor() == mOwner)
	{
		childtransform->GetRectTransform()->SetParent(nullptr);
	}
}

void RectTransform::AddParentActor(UIActorObject* parent)
{
	mParentActor = parent;
}

void RectTransform::SetParent(UIActorObject* newParent)
{
	/*
	// 1. 変更不要なケースは早期リターン
	// 同じ親を再設定しようとしている
	if (mParentActor == newParent)
	{
		return;
	}
	// 自分自身を親にしようとしている
	if (mOwner == newParent)
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
		mParentActor->GetRectTransform()->RemoveChild(mActor);
	}

	// 4. 新しい親子関係を構築する
	mParentActor = newParent;
	if (mParentActor)
	{
		mParentActor->GetRectTransform()->AddChild(mActor);
	}

	// 5. ワールドトランスフォームを維持するように、新しいローカル値を計算する
	if (mParentActor)
	{
		// 新しい親を基準にしたローカル座標を逆算する
		// NewLocal = CurrentWorld * ParentWorld^-1
		mParentActor->GetRectTransform()->ComputeWorldTransform(); // 親の行列を最新に
		Matrix4 parentWorldInverse = mParentActor->GetRectTransform()->GetWorldTransform();
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
	*/
}

void RectTransform::RemoveParentActor()
{
	SetParent(nullptr);
}

void RectTransform::SetDirty()
{
	if (mIsDirty)
	{
		return;
	}

	//1.自分自身を更新可能にする
	mIsDirty = true;

	for (auto child : mChildActor)
	{
		child->GetRectTransform()->SetDirty();//再帰的にフラグを立てる
	}
	//Sceneがあるなら
	if (SceneManager::GetNowScene() != nullptr)
	{
		SceneManager::GetNowScene()->SetDirtyFlag(true);
	}
}

void RectTransform::ActiveDirty()
{
	mIsDirty = true;
}

void RectTransform::Serialize(json& j) const
{
	Component::Serialize(j);
}

void RectTransform::Deserialize(const json& j)
{
	mIsDirty = true;
}

void RectTransform::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
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
		if (ImGui::DragFloat2("Scale", &scale.x, 0.1f))//0.1fはドラッグの感度
		{
			//ローカル関数なので注意
			SetLocalScale(scale);
		}
	}
}
