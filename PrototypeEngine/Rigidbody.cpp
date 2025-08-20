#include "Rigidbody.h"

Rigidbody::Rigidbody(ActorObject* owner, int updateOrder)
	: Component(owner)
	, mUseGravity(false)
	, mGravityScale(2)
	, mMass(1.0f)
    , mFriction(0.3f)
    , mBounciness(0.1f)
    , mSolverIterationCount(4)
{
	mUseGravity = true;
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
    Vector3 position = mOwner->GetLocalPosition();
    position += mVelocity * deltaTime;
    mOwner->SetLocalPosition(position);

    // 力のリセット
    //（次フレームでまたAddForceするため）
    mForces = Vector3::Zero;

    // 状態リセット（次回のResolveCollisionで再セットされる）
    mIsGrounded = false;
}

void Rigidbody::ResolveCollision(const Vector3& push)
{
    Vector3 pushNormal = push.Normalized();
    float normalSpeed = Vector3::Dot(mVelocity, pushNormal);

    if (normalSpeed < 0.0f)
    {
        // 接地状態を検出（法線が上向きに近い＝床）
        if (pushNormal.y > 0.7f)
        {
            mIsGrounded = true;
        }

        // ▼ バウンド（反発係数）
        Vector3 bounce = (-1.0f * pushNormal) * normalSpeed * mBounciness;

        // ▼ 摩擦：接触面と並行な速度
        Vector3 tangent = mVelocity - (pushNormal * normalSpeed);
        tangent *= (1.0f - mFriction);

        // ▼ 最終的な速度 = 反発＋摩擦
        mVelocity = bounce + tangent;

        // 小さな反発を無効に（静止判定）
        if (std::abs(Vector3::Dot(mVelocity, pushNormal)) < 0.01f)
        {
            //mVelocity -= pushNormal * Vector3::Dot(mVelocity, pushNormal);
            mVelocity = Vector3::Zero;
        }
    }
}

void Rigidbody::ApplyPushCorrection(const Vector3& correction, float dt)
{
    // correctionを速度に変換して加算（次のFixedUpdateで統合される）
    Vector3 correctionVelocity = correction / dt;

    // 法線方向の速度がめり込み方向なら置き換え or 調整
    float projected = Vector3::Dot(mVelocity, correctionVelocity.Normalized());
    if (projected < 0.0f)
    {
        mVelocity -= correctionVelocity.Normalized() * projected;
    }

    // さらに correctionVelocity を反映
    mVelocity += correctionVelocity;
}

void Rigidbody::AddForce(Vector3 force)
{
    mForces += force;
}
