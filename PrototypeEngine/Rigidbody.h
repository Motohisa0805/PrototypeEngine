#pragma once
#include "StandardLibrary.h"
#include "Math.h"
#include "Collision.h"
#include "Component.h"
#include "Actor.h"
#include "BaseScene.h"
#include "PhysWorld.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/

//オブジェクトにコンポーネントして使うRigidbody
// UnityのRigidbodyに近いクラス
class Rigidbody : public Component
{
private:
	//重力フラグ
	bool		mUseGravity;
	//重力スケーリング
	float		mGravityScale;
	//質量(1.0f = 1kg)
	float		mMass;
	//移動量
	Vector3		mVelocity;
	//加える分の力の変数
	Vector3		mForces = Vector3::Zero;

	// 0～1：静止摩擦係数
	float		mFriction;     

	// 0～1：反発係数
	float		mBounciness;   

	int			mSolverIterationCount;
	// Rigidbody.h
	bool		mIsGrounded = false;
public:
				Rigidbody(class ActorObject* owner, int updateOrder = 100);
	//FixedUpdateで呼び出す
	void		FixedUpdate(float deltaTime)override;


	void		ResolveCollision(const Vector3& push);

	void		ApplyPushCorrection(const Vector3& correction, float dt);

	//Getter
	//重力フラグの参照
	bool		IsUseGravity() { return mUseGravity; }
	//力を加える関数
	void		AddForce(Vector3 velocity);
	//mVelocityのGetter
	Vector3		GetVelocity() { return mVelocity; }
	//Setter
	//重力フラグの切り替え
	void		SetUseGravity(bool active) { mUseGravity = active; }
	//mVelocityのSetter
	void		SetVelocity(Vector3 velocity) { mVelocity = velocity; }

	float		GetFriction() const { return mFriction; }
	void		SetFriction(float f) { mFriction = f; }

	float		GetBounciness() const { return mBounciness; }
	void		SetBounciness(float b) { mBounciness = b; }

	int			GetSolverIterationCount() const { return mSolverIterationCount; }
	void		SetSolverIterationCount(int count) { mSolverIterationCount = count; }

	void		SetGrounded(bool grounded) { mIsGrounded = grounded; }
	bool		IsGrounded() const { return mIsGrounded; }
};