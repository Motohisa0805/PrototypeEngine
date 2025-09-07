#pragma once
#include "Component.h"
#include "Actor.h"
#include "BaseCamera.h"

//三人称用のオブジェクトの移動処理
class FollowObjectMovement : public Component
{
public:
	// 更新の順序を下げて最初に更新
			FollowObjectMovement(ActorObject* owner, int updateOrder = 10);
	void	FixedUpdate(float deltaTime)override;
	void	Update(float deltaTime) override;
	//基本的な移動動作を行う関数
	void	MoveInputUpdate(const struct InputState& keys);
	//***Getter***
	bool	IsJumping() { return mJumping; }
	
	Vector3 GetInputDirection() { return mInputDirection; }

	//***Setter***
	void	SetJumping(bool active) { mJumping = active; }


private:
	//移動スピード
	float	mMaxSpeed;
	
	bool	mGravity;

	bool	mJumping;

	Vector3 mVelocity;

	Vector3 mInputDirection;
};

