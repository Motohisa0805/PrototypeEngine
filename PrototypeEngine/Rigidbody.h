#pragma once
#include "Component.h"
#include "StandardLibrary.h"
#include "Math.h"
#include "Collision.h"
#include "Collider.h"
#include "Physics.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

class Entity;

//オブジェクトにコンポーネントして使うRigidbody
// UnityのRigidbodyに近いクラス
class Rigidbody : public Component
{
private:
	//重力フラグ
	bool					mUseGravity;
	bool					mIsPrivateUseGravityScale;
	//重力スケーリング
	float					mGravityScale;
	//質量(1.0f = 1kg)
	float					mMass;
	//移動量
	Vector3					mVelocity;
	//加える分の力の変数
	Vector3					mForces = Vector3::Zero;

	// 0～1：静止摩擦係数
	float					mFriction;     

	// 0～1：反発係数
	float					mBounciness;   
	// Rigidbody.h
	bool					mIsGrounded = false;

	// --- 回転運動に必要な要素 ---
	//角速度
	Vector3					mAngularVelocity;
	//トルク/回転モーメント
	Vector3					mTorques;
	// 慣性テンソル
	float					mInertia;
	//角減衰
	float					mAngularDamping;
	// 線形減衰
	float					mLinearDamping;
	// 慣性テンソルの逆行列（ワールド座標系）
	Matrix3					mInverseInertiaTensorW;
	// ローカル座標系での慣性テンソル（Fixed）
	Matrix3					mInverseInertiaTensorL;

	Collider::ColliderType	mShapeType;

	Vector3					mTempPosition;

	//スリープ状態を管理するフラグ
	bool 					mIsSleeping;

	float					mSleepTimer;

	float					mSleepThreshold;

	bool					mIsInActiveList;
public:
				 Rigidbody(Entity* owner, int updateOrder = 100);
				 ~Rigidbody();
	//FixedUpdateで呼び出す
	void		 FixedUpdate(float deltaTime)override;

	void		 UpdateSleepState(float deltaTime);

	void		 OnUpdateWorldTransform()override;

	void		 ResolveVelocity(Rigidbody* other, const Vector3& normal, const Vector3& contactPoint, float deltaTime);

	void		 ApplyImpulse(const Vector3& impulse, const Vector3& contactPoint);

	Matrix3 	 GetInverseInertiaTensorW() const { return mInverseInertiaTensorW; }

	//Getter
	//重力フラグの参照
	bool		 IsUseGravity() { return mUseGravity; }
	//力を加える関数
	void		 AddForce(Vector3 velocity);

	float 		 GetMass() { return mMass; }

	float		 GetInverseMass() const {return mMass > 0.0f ? 1.0f / mMass : 0.0f;}

	void		 SetMass(float mass) {mMass = mass; CalculateInertiaTensor();}
	//mVelocityのGetter
	Vector3		 GetVelocity() { return mVelocity; }
	//Setter
	//mVelocityのSetter
	void		 SetVelocity(Vector3 velocity) { mVelocity = velocity; }
	//重力フラグの切り替え
	void		 SetUseGravity(bool active) { mUseGravity = active; }

	float		 GetFriction() const { return mFriction; }
	void		 SetFriction(float f) { mFriction = f; }

	float		 GetBounciness() const { return mBounciness; }
	void		 SetBounciness(float b) { mBounciness = b; }

	void		 SetGrounded(bool grounded) { mIsGrounded = grounded; }
	bool		 IsGrounded() const { return mIsGrounded; }

	// 慣性モーメントの初期化処理
	void         CalculateInertiaTensor();

	Vector3		 GetAngularVelocity() const { return mAngularVelocity; }
	void		 SetAngularVelocity(const Vector3& angularVelocity) { mAngularVelocity = angularVelocity; }

	Vector3		 GetTempPosition() const { return mTempPosition; }
	void		 SetTempPosition(const Vector3& tempPosition) { mTempPosition = tempPosition; }

	bool 		 IsSleeping() const { return mIsSleeping; }
	void		 WakeUp() { mIsSleeping = false; mSleepTimer = 0.0f; }

	bool 		 IsInActiveList() const { return mIsInActiveList; }
	void 		 SetInActiveList(bool inActiveList) { mIsInActiveList = inActiveList; }

	void		 Serialize(json& j) const override;
	void		 Deserialize(const json& j)override;

	void		 DrawCustomGUI(const std::vector<PropertyInfo>& properties)override;

	Component*	 Clone(Entity* newOwner) const override;
};