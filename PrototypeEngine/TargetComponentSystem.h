#pragma once
#include "TargetComponent.h"
#include "Canvas.h"
#include "Actor.h"

//�^�[�Q�b�g�R���|�[�l���g���܂Ƃ߂Ă��擾���ĕۑ�����N���X
//���UI�̃��[�_�[�̕\���Ɏg��
class TargetComponentSystem : public ActorObject
{
private:
	vector<TargetComponent*>			mTargetComponent;
	//TargetComponent���擾����͈�
	float								mTargetRange;

public:
	TargetComponentSystem();

	void								UpdateActor(float deltaTime)override;

	void								AllTargetCheck();

	vector<TargetComponent*>			GetTargetComponent() { return mTargetComponent; }

	void								AddTargetComponent(class TargetComponent* tc);

	void								RemoveTargetComponent(class TargetComponent* tc);
};

