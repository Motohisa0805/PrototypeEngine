#include "ActorManager.h"
#include "Actor.h"

void ActorManager::UpdateActors(float time)
{
	mUpdatingActors = true;
	for (int i = 0; i < mActors.size(); i++)
	{
		if (mActors[i]->GetState() == ActorObject::EActive)
		{
			mActors[i]->Update(time);
		}
	}

	mUpdatingActors = false;

	// 保留中のアクターをmActorsに移動します
	for (int i = 0; i < mPendingActors.size(); i++)
	{
		mPendingActors[i]->GetTransform()->ComputeWorldTransform();
		mActors.emplace_back(mPendingActors[i]);
	}

	mPendingActors.clear();

	// Add any dead actors to a temp vector
	vector<ActorObject*> deadActors;
	for (int i = 0; i < mActors.size(); i++)
	{
		if (mActors[i]->GetState() == ActorObject::EDead)
		{
			deadActors.emplace_back(mActors[i]);
		}
	}


	// Delete dead actors (which removes them from mActors)
	for (auto actor : deadActors)
	{
		delete actor;
	}
}

void ActorManager::FixedUpdateActors(float time)
{
	for (auto actor : mActors)
	{
		actor->FixedUpdate(time);
	}
}

void ActorManager::UnloadActors()
{
	// Delete actors
// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}
	mActors.clear();
}

void ActorManager::AddActor(ActorObject* actor)
{
	if (mUpdatingActors)
	{
		// If we're updating actors, need to add to pending
		mPendingActors.emplace_back(actor);
	}
	else
	{
		// 更新中でない場合はメインリストに直接追加（エディタ操作は通常こちら）
		mActors.push_back(actor);
	}
}

void ActorManager::RemoveActor(ActorObject* actor)
{
	// Is it in pending actors?
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	// Is it in actors?
	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

void ActorManager::DeleteActor(ActorObject* actor)
{
	actor->SetState(ActorObject::EDead);
}