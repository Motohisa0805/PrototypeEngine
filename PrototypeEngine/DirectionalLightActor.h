#pragma once
#include "Actor.h"
#include "DirectionalLightComponent.h"

//�����̃I�u�W�F�N�g
//Unity�Ɠ����悤�ɉ�]�ʂŖ��邳���ς��
class DirectionalLightActor : public ActorObject
{
private:
	DirectionalLightComponent* mDirectionalLightComp;
public:
	DirectionalLightActor();
	~DirectionalLightActor();

	void ActorInput(const struct InputState& keyState)override;
};
