#pragma once
#include "Component.h"
#include "Actor.h"

//ボタンを押した時での操作に特化した移動機能
//現在はテストなので矢印キーで移動
class PushButtonMove : public Component
{
public:
	// 更新の順序を下げて最初に更新
			PushButtonMove(class ActorObject* owner, int updateOrder = 10);
	void	Update(float deltaTime) override;
	//基本的な移動動作を行う関数
	void	MoveInputUpdate(const struct InputState& keys);
	//***Setter***

	void	SetSpeed(float speed) { mSpeed = speed; }

	void	SetForwardSpeed(float speed) { mForwardSpeed = speed; }

	void	SetStrafeSpeed(float speed) { mStrafeSpeed = speed; }

	void	AddJumpPower(float speed) { mJumpPower = speed; }

	
private:
	//上下左右で使用する移動量
	float	mSpeed;

	float	mForwardSpeed;

	float	mStrafeSpeed;

	float	mJumpPower;
};

