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
	, mIsPrivateUseGravityScale(false)
    , mGravityScale(1.0f)
    , mMass(1.0f)
    , mFriction(0.3f)
    , mBounciness(0.1f)
    , mVelocity(Vector3::Zero)
    , mForces(Vector3::Zero)
    , mAngularVelocity(Vector3::Zero)
    , mTorques(Vector3::Zero)
    , mInertia(1.0f)
    , mAngularDamping(0.998f)
	, mLinearDamping(0.998f)
    , mInverseInertiaTensorW(Matrix3::Identity)
    , mInverseInertiaTensorL(Matrix3::Identity)
    , mShapeType(Collider::ColliderType::SphereType) // 仮の初期化
	, mTempPosition(Vector3::Zero)
	, mIsSleeping(false)
	, mSleepTimer(0.0f)
	, mSleepThreshold(0.5f)
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
	UpdateSleepState(deltaTime);
    
    if (mIsSleeping) return;

    Vector3 gravityForce;
    //重力フラグが有効なら
    if (mUseGravity && !mIsGrounded)
    {
		// 重力スケーリングの適用
		// mIsPrivateUseGravityScale が true の場合は mGravityScale を使用し、そうでない場合は 1.0f を使用
		float gravityScale = mIsPrivateUseGravityScale ? mGravityScale : 1.0f;
        // 重力を力として加える
        gravityForce = Vector3::NegUnitY * Physics::GRAVITY_ACCELERATION * mMass * gravityScale;
    }
    mForces += gravityForce;

    // 運動方程式: F = ma → a = F / m
    Vector3 acceleration = mForces / mMass;

    // 速度更新
    mVelocity += acceleration * deltaTime;

    mVelocity *= (1.0f - mLinearDamping * deltaTime);

    // --- 微小な動きを完全に止める（スリープ） ---
    if (mVelocity.LengthSq() < 0.001f) mVelocity = Vector3::Zero;

    // 位置更新
    Vector3 position = mOwner->GetTransform()->GetPosition();
    position += mVelocity * deltaTime;
    mOwner->GetTransform()->SetLocalPosition(position);

    // --- 角運動の更新 ---
    if (mMass > 0.0f)
    {
        Quaternion rotation = mOwner->GetTransform()->GetLocalRotation();

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
        if (mAngularVelocity.LengthSq() < 0.001f) mAngularVelocity = Vector3::Zero;

        // 5. 回転（クォータニオン）の更新
        if (mAngularVelocity.LengthSq() > 1e-6f)
        {
            // オイラー積分: q_new = q_old + (dt * 0.5f) * (pureQuaternion * q_old)
            // 角速度ベクトルから「純粋クォータニオン」（w=0）を作成
            //q_old
			Quaternion oldRotation = mOwner->GetTransform()->GetLocalRotation();
            
            //(dt * 0.5f)
			float dt = deltaTime * 0.5f;

            //pureQuaternion
            Quaternion pureQuaternion(mAngularVelocity.x, mAngularVelocity.y, mAngularVelocity.z, 0.0f);
            // (pureQuaternion * rotation) は四元数の積を指します
            //(pureQuaternion * q_old)
            Quaternion deltaRotation = pureQuaternion * oldRotation;

            // 時間ステップを適用し、現在の回転に加算
			//(dt * 0.5f) * (pureQuaternion * q_old)
            deltaRotation.x *= dt;
            deltaRotation.y *= dt;
            deltaRotation.z *= dt;
            deltaRotation.w *= dt;

            // クォータニオンの足し算
			Quaternion newRotation = oldRotation + deltaRotation;
            newRotation.Normalize();
            mOwner->GetTransform()->SetLocalRotation(newRotation);
            //GUI上で編集する用キャッシュ数値をVector3で取得
            Vector3 eulerRad = newRotation.ToEulerAngles();
            Vector3 rotEuler;
            rotEuler.x = Math::ToDegrees(eulerRad.x);
            rotEuler.y = Math::ToDegrees(eulerRad.y);
            rotEuler.z = Math::ToDegrees(eulerRad.z);
            mOwner->GetTransform()->SetRotationEditor(rotEuler);
        }
        // 力のリセット
        //（次フレームでまたAddForceするため）
        mForces = Vector3::Zero;

        // 状態リセット（次回のResolveCollisionで再セットされる）
        mIsGrounded = false;
        // 6. トルクをリセット
        mTorques = Vector3::Zero;
    }

    //最大速度の制御
	float maxSpeed = 40.0f; 
    if (mVelocity.LengthSq() > maxSpeed * maxSpeed) {
        mVelocity.Normalize();
		mVelocity *= maxSpeed;
    }

}

void Rigidbody::UpdateSleepState(float deltaTime)
{
    // 速度と角速度がしきい値以下かチェック
    if (mVelocity.LengthSq() < 0.01f && mAngularVelocity.LengthSq() < 0.01f) {
        mSleepTimer += deltaTime;
        if (mSleepTimer > 0.5f) { // 0.5秒静止したら
            mIsSleeping = true;
            mVelocity = Vector3::Zero;
            mAngularVelocity = Vector3::Zero;
        }
    }
    else {
        // 動いているならタイマーリセット
        mSleepTimer = 0.0f;
        mIsSleeping = false;
    }
}

void Rigidbody::OnUpdateWorldTransform()
{
    CalculateInertiaTensor();
}

void Rigidbody::ResolveVelocity(Rigidbody* other, const Vector3& normal, const Vector3& contactPoint, float deltaTime)
{
    // ==========================================
    // 0. 接地判定 (mIsGrounded) の処理
    // ==========================================
    // normalは「相手から自分(this)に向かう法線」です。
    // y成分が一定以上なら「床の上にいる」と判定します。
    if (normal.y > 0.7f)
    {
        mIsGrounded = true;
    }
    // もし相手が動的オブジェクトで、自分が相手の上に乗ったなら、相手も接地状態にする
    if (other && normal.y < -0.7f)
    {
        // ※Rigidbodyに mIsGrounded を直接操作できるセッターがある前提です
        other->SetGrounded(true); 
    }

    // ==========================================
    // 1. 相対速度の計算
    // ==========================================
    Vector3 rA = contactPoint - mOwner->GetTransform()->GetPosition();
    Vector3 vContactA = mVelocity + Vector3::Cross(mAngularVelocity, rA);

    Vector3 rB = Vector3::Zero;
    Vector3 vContactB = Vector3::Zero;
    if (other) {
        rB = contactPoint - other->GetOwner()->GetTransform()->GetPosition();
        vContactB = other->GetVelocity() + Vector3::Cross(other->GetAngularVelocity(), rB);
    }

    Vector3 relativeVelocity = vContactA - vContactB;
    float vRelN = Vector3::Dot(relativeVelocity, normal);

    // 離れようとしている場合はインパルスを加えない
    if (vRelN > 0.0f) return;

    // ==========================================
    // 2. 法線方向（反発）インパルスの計算
    // ==========================================
    float bounciness = mBounciness; // (本来は std::min(mBounciness, other->mBounciness) が自然)

    if (std::abs(vRelN) < 1.0f)
    {
        bounciness = 0.0f;
    }

    // mMassそのものではなく、質量の逆数（1/m）を使う
    float invMassA = GetInverseMass();
    float invMassB = other ? other->GetInverseMass() : 0.0f;

    // Aの回転しにくさ（有効質量）
    Vector3 rnA = Vector3::Cross(rA, normal);
    Vector3 invI_rnA = mInverseInertiaTensorW.Transform(rnA);
    float kNormalA = invMassA + Vector3::Dot(normal, Vector3::Cross(invI_rnA, rA));

    // Bの回転しにくさ（有効質量）
    float kNormalB = 0.0f;
    if (other) {
        Vector3 rnB = Vector3::Cross(rB, normal);
        Vector3 invI_rnB = other->GetInverseInertiaTensorW().Transform(rnB);
        kNormalB = invMassB + Vector3::Dot(normal, Vector3::Cross(invI_rnB, rB));
    }

    // 総和で割る
    float kNormalSum = kNormalA + kNormalB;
    float jn = -(1.0f + bounciness) * (vRelN / kNormalSum);

    Vector3 impulseN = normal * jn;

    // インパルス適用
    ApplyImpulse(impulseN, contactPoint);
    if (other) {
        other->ApplyImpulse(impulseN * -1.0f, contactPoint); // Bには逆向きの力
    }

    // ==========================================
    // 3. 接線方向（摩擦）インパルスの計算
    // ==========================================
    // 反発適用後の「最新の速度」で再計算する（ここが元のコードの素晴らしい点です）
    vContactA = mVelocity + Vector3::Cross(mAngularVelocity, rA);
    if (other) {
        vContactB = other->GetVelocity() + Vector3::Cross(other->GetAngularVelocity(), rB);
    }

    relativeVelocity = vContactA - vContactB;
    float vRelNAfter = Vector3::Dot(relativeVelocity, normal);

    // 接線ベクトル (vt)
    Vector3 vt = relativeVelocity - (normal * vRelNAfter);

    if (vt.LengthSq() > 0.0001f)
    {
        Vector3 t = vt.Normalized();

        Vector3 rtA = Vector3::Cross(rA, t);
        Vector3 invI_rtA = mInverseInertiaTensorW.Transform(rtA);
        float kTangentA = invMassA + Vector3::Dot(t, Vector3::Cross(invI_rtA, rA));

        float kTangentB = 0.0f;
        if (other) {
            Vector3 rtB = Vector3::Cross(rB, t);
            Vector3 invI_rtB = other->GetInverseInertiaTensorW().Transform(rtB);
            kTangentB = invMassB + Vector3::Dot(t, Vector3::Cross(invI_rtB, rB));
        }

        float kTangentSum = kTangentA + kTangentB;
        float jt = -Vector3::Dot(relativeVelocity, t) / kTangentSum;

        // クーロン摩擦制限
        float maxFriction = mFriction * std::abs(jn); // (std::min(mFriction, other->mFriction) が理想)
        jt = Math::Clamp(jt, -maxFriction, maxFriction);

        Vector3 impulseT = t * jt;

        // インパルス適用
        ApplyImpulse(impulseT, contactPoint);
        if (other) {
            other->ApplyImpulse(impulseT * -1.0f, contactPoint);
        }
    }
}

void Rigidbody::ApplyImpulse(const Vector3& impulse, const Vector3& contactPoint)
{
    // 1. 線形速度への影響
    mVelocity += impulse * (1.0f / mMass);

    // 2. 角速度への影響（ここが回転の源！）
    // 重心から衝突点へのベクトル
    Vector3 r = contactPoint - mOwner->GetTransform()->GetPosition();

    // トルク成分 = r × impulse (外積)
    Vector3 impulsiveTorque = Vector3::Cross(r, impulse);

    // 角速度の更新：ω += I^-1 * (r × j)
    // ※現在の慣性テンソル（ワールド空間）を掛ける
    mAngularVelocity += mInverseInertiaTensorW.Transform(impulsiveTorque);
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
	j["IsPrivateUseGravityScale"] = mIsPrivateUseGravityScale;
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
    if (j.contains("IsPrivateUseGravityScale"))
    {
        mIsPrivateUseGravityScale = j.at("IsPrivateUseGravityScale").get<bool>();
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

	//個別の重力スケールを使用するかどうかのチェックボックス
	ImGui::Checkbox("Use Private Gravity Scale", &mIsPrivateUseGravityScale);
	//重力スケールのドラッグフロート。個別の重力スケールを使用する場合は有効、そうでない場合は無効にする
    ImGui::BeginDisabled(!mIsPrivateUseGravityScale);
	//個別の重力スケールを使用しない場合は、全てのRigidbodyで同じ重力スケールを使用するため、ここで編集できるようにする
    ImGui::DragFloat("GravityScale", &mGravityScale);
    ImGui::EndDisabled();

    ImGui::NewLine();
    ImGui::SetNextItemWidth(50);
    ImGui::DragFloat("Friction", &mFriction);
    ImGui::NewLine();
    ImGui::SetNextItemWidth(50);
    ImGui::DragFloat("Bounciness", &mBounciness);
    ImGui::NewLine();

    if (mIsSleeping)
    {
		ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Sleeping");
    }

    ImGui::Separator();
}
