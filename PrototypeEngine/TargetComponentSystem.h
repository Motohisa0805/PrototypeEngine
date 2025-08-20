#pragma once
#include "TargetComponent.h"
#include "Canvas.h"
#include "Actor.h"

//ターゲットコンポーネントをまとめてい取得して保存するクラス
//主にUIのレーダーの表示に使う
class TargetComponentSystem : public ActorObject
{
private:
	vector<TargetComponent*>			mTargetComponent;
	//TargetComponentを取得する範囲
	float								mTargetRange;

public:
	TargetComponentSystem();

	void								UpdateActor(float deltaTime)override;

	void								AllTargetCheck();

	vector<TargetComponent*>			GetTargetComponent() { return mTargetComponent; }

	void								AddTargetComponent(class TargetComponent* tc);

	void								RemoveTargetComponent(class TargetComponent* tc);
};

