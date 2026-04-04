#include "Rigidbody.h"
#include "Actor.h"
#include "BaseScene.h"
#include "PhysWorld.h"

#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "Time.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

Rigidbody::Rigidbody(ActorObject* owner, int updateOrder)
    : Component(owner)
    , mUseGravity(false)
    , mGravityScale(2)
    , mMass(1.0f)
    , mFriction(0.3f)
    , mBounciness(0.1f)
    , mVelocity(Vector3::Zero)
    , mForces(Vector3::Zero)
    , mAngularVelocity(Vector3::Zero)
    , mTorques(Vector3::Zero)
    , mInertia(1.0f)
    , mAngularDamping(1.0f)
    , mInverseInertiaTensorW(Matrix3::Identity)
    , mInverseInertiaTensorL(Matrix3::Identity)
    , mShapeType(Collider::ColliderType::SphereType) // 仮の初期化
{
    mName = "Rigidbody";
	mUseGravity = true;

    CalculateInertiaTensor();

    mHeaderColor = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
    mHeaderHoveredColor = Vector4(0.3f, 0.6f, 0.3f, 1.0f);
    mHeaderActiveColor = Vector4(0.4f, 0.8f, 0.4f, 1.0f);
}

void Rigidbody::FixedUpdate(float deltaTime)
{
    Vector3 gravityForce;
    //重力フラグが有効なら
    if (mUseGravity && !mIsGrounded)
    {
        // 重力を力として加える
        gravityForce = Vector3::UnitY * (-9.8f) * mMass * mGravityScale;
    }
    mForces += gravityForce;

    // 運動方程式: F = ma → a = F / m
    Vector3 acceleration = mForces / mMass;

    // 速度更新
    mVelocity += acceleration * deltaTime;

    // 位置更新
    Vector3 position = mOwner->GetTransform()->GetLocalPosition();
    position += mVelocity * deltaTime;
    mOwner->GetTransform()->SetLocalPosition(position);

    // --- 角運動の更新 ---
    if (mMass > 0.0f)
    {
        Quaternion rotation = mOwner->GetTransform()->GetRotation();

        // 1. ワールド逆慣性テンソルの更新
        // I_world^-1 = R * I_local^-1 * R^T
        Matrix3 R = Matrix3(rotation); // Quaternionから回転行列に変換
        Matrix3 RT = R.Transpose();
        mInverseInertiaTensorW = R * mInverseInertiaTensorL * RT;

        // 2. 角加速度の計算: α = I_world^-1 * τ
        Vector3 angularAcceleration = mInverseInertiaTensorW.Transform(mTorques);

        // 3. 角速度の更新
        mAngularVelocity += angularAcceleration * deltaTime;

        // 4. 角減衰の適用
        mAngularVelocity *= (1.0f - mAngularDamping * deltaTime);

        // 5. 回転（クォータニオン）の更新
        if (mAngularVelocity.LengthSq() > 1e-6f)
        {
            // オイラー積分: q_new = q_old + (dt * 0.5f) * (pureQuaternion * q_old)
            // 角速度ベクトルから「純粋クォータニオン」（w=0）を作成
            //pureQuaternion
            Quaternion pureQuaternion(mAngularVelocity.x, mAngularVelocity.y, mAngularVelocity.z, 0.0f);
            //q_old
            Quaternion rotation = mOwner->GetTransform()->GetRotation();

            // (pureQuaternion * rotation) は四元数の積を指します
            //(pureQuaternion * q_old)
            Quaternion deltaRotation = rotation * pureQuaternion;

            // 時間ステップを適用し、現在の回転に加算
            //(dt * 0.5f)
            deltaRotation.x *= (0.5f * deltaTime);
            deltaRotation.y *= (0.5f * deltaTime);
            deltaRotation.z *= (0.5f * deltaTime);
            deltaRotation.w *= (0.5f * deltaTime);

            // クォータニオンの足し算
            rotation = rotation + deltaRotation;
            rotation.Normalize();

            mOwner->GetTransform()->SetRotation(rotation);
        }
        // 力のリセット
        //（次フレームでまたAddForceするため）
        mForces = Vector3::Zero;

        // 状態リセット（次回のResolveCollisionで再セットされる）
        mIsGrounded = false;
        // 6. トルクをリセット
        mTorques = Vector3::Zero;
    }
}

void Rigidbody::OnUpdateWorldTransform()
{
    CalculateInertiaTensor();
}

void Rigidbody::ResolveCollision(const Vector3& contactNormal,const Vector3& contactPoint,float penetrationDepth)
{
    // 衝突法線（床 → 剛体）
    Vector3 hitNormal = contactNormal;

    Vector3 centerOfMass = mOwner->GetTransform()->GetPosition();
    Vector3 r = contactPoint - centerOfMass;

    // 接触点速度
    Vector3 vContact = mVelocity + Vector3::Cross(mAngularVelocity, r);

    // 法線方向相対速度
    float vRelN = Vector3::Dot(vContact, hitNormal);

    // 離れているなら何もしない
    if (vRelN > 0.0f)
    {
        return;
    }

    //法線 impulse（反発）
    float bounciness = mBounciness;

    Vector3 rn = Vector3::Cross(r, hitNormal);
    Vector3 invI_rn = mInverseInertiaTensorW.Transform(rn);

    float kNormal = (1.0f / mMass) + Vector3::Dot(hitNormal, Vector3::Cross(invI_rn, r));

    float jn = -(1.0f + bounciness) * (vRelN / kNormal);

    Vector3 impulseN = hitNormal * jn;

    // 並進
    mVelocity += impulseN / mMass;
    // 回転
    mAngularVelocity += mInverseInertiaTensorW.Transform(Vector3::Cross(r, impulseN));

    // --- 法線 impulse 後の接触点速度を再計算 ---
    Vector3 vContactAfter =mVelocity + Vector3::Cross(mAngularVelocity, r);

    float vRelNAfter = Vector3::Dot(vContactAfter, hitNormal);

    // =========================
    // 2. 摩擦 impulse
    // =========================
    Vector3 vt = vContactAfter - (hitNormal * vRelNAfter);

    if (vt.LengthSq() > 1e-6f)
    {
        Vector3 t = vt.Normalized();

        Vector3 rt = Vector3::Cross(r, t);
        Vector3 invI_rt = mInverseInertiaTensorW.Transform(rt);

        float kTangent = (1.0f / mMass) + Vector3::Dot(t, Vector3::Cross(invI_rt, r));

        float jt = -Vector3::Dot(vContactAfter, t) / kTangent;

        // クーロン摩擦制限
        float maxFriction = mFriction * std::abs(jn);
        jt = Math::Clamp(jt, -maxFriction, maxFriction);

        Vector3 impulseT = t * jt;

        // 並進
        mVelocity += impulseT / mMass;
        // 回転
        mAngularVelocity += mInverseInertiaTensorW.Transform(Vector3::Cross(r, impulseT));
    }

    // =========================
    // 3. penetration correction（姿勢安定用）
    // =========================
    if (penetrationDepth > 0.0f)
    {
        const float percent = 0.8f;     // 押し戻し率
        const float slop = 0.01f;       // 許容めり込み

        float depth = Math::Max(penetrationDepth - slop, 0.0f);
        Vector3 correction = hitNormal * (depth * percent);

        mOwner->GetTransform()->SetLocalPosition(mOwner->GetTransform()->GetPosition() + correction);
    }

    // 接地判定
    if (hitNormal.y > 0.7f)
    {
        mIsGrounded = true;
    }
}

void Rigidbody::AddForce(Vector3 force)
{
    mForces += force;
}

void Rigidbody::CalculateInertiaTensor()
{
    //コライダーの取得
    Collider* coll = GetOwner()->GetComponent<Collider>();
    if (!coll)return;

    Matrix3 inertiaTensor = Matrix3::Identity;
    float mass = mMass;

    if (coll->GetType() == Collider::SphereType)
    {
        SphereCollider* sc = static_cast<SphereCollider*>(coll);
        float R = sc->GetWorldSphere().mRadius;

        //球体：I = 2/5 * M * R^2.軸対象なので対角成分にスカラー値を設定
        float I_scalar = (2.0f / 5.0f) * mass * (R * R);
        inertiaTensor.mat[0][0] = inertiaTensor.mat[1][1] = inertiaTensor.mat[2][2] = I_scalar;
        mShapeType = coll->GetType();
    }
    else if (coll->GetType() == Collider::BoxType)
    {
        BoxCollider* bc = static_cast<BoxCollider*>(coll);

        // ローカルサイズ（回転・スケールなし）
        Vector3 halfSize = bc->GetWorldOBB().mExtents;

        float W = halfSize.x * 2.0f;
        float H = halfSize.y * 2.0f;
        float D = halfSize.z * 2.0f;

        inertiaTensor.mat[0][0] = (1.0f / 12.0f) * mMass * (H * H + D * D);
        inertiaTensor.mat[1][1] = (1.0f / 12.0f) * mMass * (W * W + D * D);
        inertiaTensor.mat[2][2] = (1.0f / 12.0f) * mMass * (W * W + H * H);
        mShapeType = coll->GetType();
    }
    else if (coll->GetType() == Collider::CapsuleType)
    {
        CapsuleCollider* cc = static_cast<CapsuleCollider*>(coll);
        // 簡単化のため、ここでBoxと同じ構造を使う（軸対称を利用できる場合はその計算を行う）
        float R = cc->GetWorldCapsule().mRadius;
        // 軸に垂直な慣性モーメントとして R^2 に依存した値を設定
        float I_perp = (2.0f / 5.0f) * mass * (R * R);
        inertiaTensor.mat[0][0] = I_perp;
        inertiaTensor.mat[2][2] = I_perp;
        // Y軸周りの回転は無視するため非常に大きな値に設定 (実際にはテンソルを使わず、FixedUpdateで処理することも可能)
        inertiaTensor.mat[1][1] = 1.0e+6f;
        mShapeType = coll->GetType();
    }
    // 逆行列を計算し、ローカル逆慣性テンソルとして格納
    mInverseInertiaTensorL = inertiaTensor.Inverse();
}

void Rigidbody::Serialize(json& j) const
{
	Component::Serialize(j);

	j["UseGravity"] = mUseGravity;
	j["GravityScale"] = mGravityScale;
	j["Mass"] = mMass;
	j["Friction"] = mFriction;
	j["Bounciness"] = mBounciness;
}

void Rigidbody::Deserialize(const json& j)
{
	Component::Deserialize(j);

    if (j.contains("UseGravity"))
    {
        mUseGravity = j.at("UseGravity").get<bool>();
	}
    if (j.contains("GravityScale"))
    {
        mGravityScale = j.at("GravityScale").get<float>();
    }
    if (j.contains("Mass"))
    {
        mMass = j.at("Mass").get<float>();
    }
    if (j.contains("Friction"))
    {
        mFriction = j.at("Friction").get<float>();
    }
    if (j.contains("Bounciness"))
    {
        mBounciness = j.at("Bounciness").get<float>();
	}
}

void Rigidbody::DrawCustomGUI(const std::vector<PropertyInfo>& properties)
{
    ImGui::Text("Rigidbody Properties");
    ImGui::NewLine();
    ImGui::Checkbox("Use Gravity",&mUseGravity);
    ImGui::NewLine();
    ImGui::SetNextItemWidth(50);
    ImGui::DragFloat("Mass", &mMass);
    ImGui::NewLine();
    ImGui::SetNextItemWidth(50);
    ImGui::DragFloat("GravityScale", &mGravityScale);
    ImGui::NewLine();
    ImGui::SetNextItemWidth(50);
    ImGui::DragFloat("Friction", &mFriction);
    ImGui::NewLine();
    ImGui::SetNextItemWidth(50);
    ImGui::DragFloat("Bounciness", &mBounciness);
    ImGui::NewLine();

    ImGui::Separator();
}
