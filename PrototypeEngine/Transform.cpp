#include "Transform.h"
#include "Actor.h"
#include "SceneManager.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"

void Transform::AddChild(ActorObject* child)
{
    // 重複チェック
    auto iter = std::find(mChildActor.begin(), mChildActor.end(), child);
    if (iter == mChildActor.end())
    {
        mChildActor.push_back(child);
    }
}

void Transform::RemoveChild(ActorObject* child)
{
    // 重複チェック
    auto iter = std::find(mChildActor.begin(), mChildActor.end(), child);
    if (iter != mChildActor.end())
    {
        mChildActor.erase(iter);
    }
}

Transform::Transform(ActorObject* owner)
    : BaseTransform(owner), mParentActor(nullptr), mChildActor()
{
    mName = "Transform";

    mHeaderColor        = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
    mHeaderHoveredColor = Vector4(0.3f, 0.6f, 0.3f, 1.0f);
    mHeaderActiveColor  = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
}

Transform::~Transform() {}

void Transform::ComputeWorldTransform()
{
    // 更新フラグがfalseなら
    if (!mIsDirty)
    {
        return;
    }

    // ローカル座標計算
    mLocalTransform = Matrix4::CreateScale(mLocalScale);
    mLocalTransform *= Matrix4::CreateFromQuaternion(mLocalRotation);
    mLocalTransform *= Matrix4::CreateTranslation(mLocalPosition);

    // 親がいたら、親のワールド行列を掛ける
    if (mParentActor)
    {
        // 親のワールドトランスフォームが最新であることを保証する必要がある
        mParentActor->GetTransform()->ComputeWorldTransform();
        mWorldTransform =
            mLocalTransform * mParentActor->GetTransform()->GetWorldTransform();
    }
    // いなかったら
    else
    {
        mWorldTransform = mLocalTransform;
    }
    mPosition = mWorldTransform.GetTranslation();
    mRotation = mWorldTransform.RemoveScale().GetRotation();
    mScale    = mWorldTransform.GetScale();

    mIsDirty = false;

    // コンポーネントの行列を更新
    for (auto comp : mOwner->GetComponents())
    {
        comp->OnUpdateWorldTransform();
    }
}

const ActorObject* Transform::GetChildActor(ActorObject* actor)
{
    for (ActorObject* a : mChildActor)
    {
        if (a == actor)
        {
            return a;
        }
    }
    return nullptr;
}

void Transform::AddChildActor(ActorObject* child)
{
    if (child)
    {
        child->GetTransform()->SetParent(mActor);
    }
}

void Transform::RemoveChildActor(ActorObject* child)
{
    if (child && child->GetTransform()->GetParentActor() == mOwner)
    {
        child->GetTransform()->SetParent(nullptr);
    }
}

void Transform::AddParentActor(ActorObject* parent) { mParentActor = parent; }

void Transform::SetParent(ActorObject* newParent)
{
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
    // 構築直前に座標を変更していた場合の処理
    ComputeWorldTransform(); // 最新の状態を計算
    Matrix4 worldMatrix = GetWorldTransform();

    // 3. 現在の親がいる場合は、その親の子リストから自分を削除する
    if (mParentActor)
    {
        mParentActor->GetTransform()->RemoveChild(mActor);
    }

    // 4. 新しい親子関係を構築する
    mParentActor = newParent;
    if (mParentActor)
    {
        mParentActor->GetTransform()->AddChild(mActor);
    }

    // 5. ワールドトランスフォームを維持するように、新しいローカル値を計算する
    if (mParentActor)
    {
        // 新しい親を基準にしたローカル座標を逆算する
        // NewLocal = CurrentWorld * ParentWorld^-1
        Matrix4 parentWorldInverse =
            mParentActor->GetTransform()->GetWorldTransform();
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

void Transform::RemoveParentActor() { SetParent(nullptr); }

void Transform::SetDirty()
{
    if (mIsDirty)
    {
        return;
    }

    // 1.自分自身を更新可能にする
    mIsDirty = true;

    for (auto child : mChildActor)
    {
        child->GetTransform()->SetDirty(); // 再帰的にフラグを立てる
    }
    // Sceneがあるなら
    if (SceneManager::GetCurrentRunScene() != nullptr)
    {
        SceneManager::GetCurrentRunScene()->SetDirtyFlag(true);
    }
}

void Transform::ActiveDirty() { mIsDirty = true; }

void Transform::Serialize(json& j) const { BaseTransform::Serialize(j); }

void Transform::Deserialize(const json& j)
{
    BaseTransform::Deserialize(j);
    mIsDirty = true;
}

void Transform::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
    // Position(Vector3)の編集
    Vector3 pos = mLocalPosition;
    if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) // 0.1fはドラッグの感度
    {
        // ローカル関数なので注意
        SetLocalPosition(pos);
    }
    // 回転だけローカルで取得
    // ローカルならスケール値を含まないため
    // 度数法で表示・編集
    Vector3 rot = GetLocalEulerAngles();
    if (ImGui::DragFloat3("Rotation(deg)", &rot.x, 1.0f))
    {
        SetLocalEulerAngles(rot);
    }

    // Scale(Vector3)の編集
    Vector3 scale = mLocalScale;
    if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) // 0.1fはドラッグの感度
    {
        // ローカル関数なので注意
        SetLocalScale(scale);
    }
}
