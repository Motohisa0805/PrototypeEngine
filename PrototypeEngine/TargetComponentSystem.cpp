#include "TargetComponentSystem.h"

TargetComponentSystem::TargetComponentSystem()
	:ActorObject()
	, mTargetRange(20.0f)
{

}

void TargetComponentSystem::UpdateActor(float deltaTime)
{
	AllTargetCheck();
}

void TargetComponentSystem::AllTargetCheck()
{
	vector<ActorObject*> actors = mGame->SelectAllActorComponent<TargetComponent>();



	for (auto actor : actors)
	{
		auto tc = actor->GetComponent<TargetComponent>();
		Vector3 dis = mGame->GetPlayer()->GetPosition() - actor->GetPosition();
		if (dis.Length() > mTargetRange)
		{
			//範囲外ならスキップ
			RemoveTargetComponent(tc);
			continue;
		}
		else
		{
			if (std::find(mTargetComponent.begin(), mTargetComponent.end(), tc) != mTargetComponent.end())
			{
				// target はすでに vector に存在する
				continue;
			}
			else
			{
				// 存在しないので追加するなど
				AddTargetComponent(tc);
			}
		}
	}
}

void TargetComponentSystem::AddTargetComponent(TargetComponent* tc)
{
	mTargetComponent.emplace_back(tc);
}

void TargetComponentSystem::RemoveTargetComponent(TargetComponent* tc)
{
	auto iter = std::find(mTargetComponent.begin(), mTargetComponent.end(),
		tc);
	if (iter != mTargetComponent.end())
	{
		mTargetComponent.erase(iter);
	}
}