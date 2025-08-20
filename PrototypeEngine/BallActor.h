#pragma once
#include "Actor.h"
//未使用
//今後使用出来るようにするために残しています。
class BallActor : public ActorObject
{
public:
	BallActor();

	void UpdateActor(float deltaTime) override;

	void SetPlayer(ActorObject* player);

	void HitTarget();
private:
	class AudioComponent*	mAudioComp;

	class BallMove*			mMyMove;
	
	float					mLifeSpan;
};