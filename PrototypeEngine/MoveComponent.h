#pragma once
#include "Component.h"
#include "Actor.h"

//参考書産のオブジェクト移動機能
//現在は未使用
class MoveComponent : public Component
{
public:
	// 更新の順序を下げて最初に更新
	MoveComponent(class ActorObject* owner, int updateOrder = 10);
	void Update(float deltaTime) override;

	float GetAngularSpeed() const { return mAngularSpeed; }
	float GetForwardSpeed() const { return mForwardSpeed; }
	float GetStrafeSpeed() const { return mStrafeSpeed; }
	void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
	void SetForwardSpeed(float speed) { mForwardSpeed = speed; }
	void SetStrafeSpeed(float speed) { mStrafeSpeed = speed; }
	void SetYSpeed(float speed) { mYSpeed = speed; }
private:
	float mAngularSpeed;
	float mForwardSpeed;
	float mStrafeSpeed;
	float mYSpeed;
};