#pragma once
#include "MoveComponent.h"

//弾の移動処理に使用していた
//現在は未使用
class BallMove : public MoveComponent
{
public:
	BallMove(class ActorObject* owner);

	void SetPlayer(ActorObject* player) { mPlayer = player; }
	void Update(float deltaTime) override;
protected:
	class ActorObject* mPlayer;
};

