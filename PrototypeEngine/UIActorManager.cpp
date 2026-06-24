#include "UIActorManager.h"
#include "UIActor.h"

UIActorManager::UIActorManager()
	: mPendingActors()
	, mActors()
	, mIsUpdatingActors(false)
{
}

void UIActorManager::UpdateActors(float time)
{
	mIsUpdatingActors = true;
	for (int i = 0; i < mActors.size(); i++)
	{
		if (mActors[i]->GetState() == UIActorObject::EActive)
		{
			mActors[i]->Update(time);
		}
	}

	mIsUpdatingActors = false;

	// 保留中のアクターをmActorsに移動します
	for (int i = 0; i < mPendingActors.size(); i++)
	{
		mPendingActors[i]->GetRectTransform()->ComputeWorldTransform();
		mActors.emplace_back(mPendingActors[i]);
	}
	mPendingActors.clear();
	//アクターの状態を更新します（EDeadになったものは後で削除）
	for (int i = 0; i < mActors.size(); i++)
	{
		mActors[i]->StateUpdate(time);
	}
	// Add any dead actors to a temp vector
	vector<UIActorObject*> deadActors;
	for (int i = 0; i < mActors.size(); i++)
	{
		if (mActors[i]->GetState() == UIActorObject::EDead)
		{
			deadActors.emplace_back(mActors[i]);
		}
	}


	// Delete dead actors (which removes them from mActors)
	for (auto actor : deadActors)
	{
		actor->OnDestroy();
		delete actor;
	}
}

void UIActorManager::FixedUpdateActors(float time)
{
	for (auto actor : mActors)
	{
		actor->FixedUpdate(time);
	}
}

void UIActorManager::UnloadActors()
{
	//全てのアクターを順番に破棄
	for (auto actor : mActors)
	{
		// デストラクタ内で Manager を呼ばないため、ここで安全に delete できる
		delete actor;
	}

	//リストを空にする
	mActors.clear();
}

void UIActorManager::AddActor(UIActorObject* actor)
{
	if (mIsUpdatingActors)
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

void UIActorManager::RemoveActor(UIActorObject* actor)
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

void UIActorManager::DeleteActor(UIActorObject* actor)
{
	actor->SetState(UIActorObject::EDead);
}

void UIActorManager::ReAddActor(UIActorObject* actor)
{
	if (mIsUpdatingActors)
	{
		// If we're updating actors, need to add to pending
		mPendingActors.emplace_back(actor);
	}
	else
	{
		// 更新中でない場合はメインリストに直接追加（エディタ操作は通常こちら）
		mActors.push_back(actor);
		actor->OnEnabled();
	}
}

void UIActorManager::DetachActor(UIActorObject* actor)
{
	auto it = std::find(mActors.begin(), mActors.end(), actor);
	if (it != mActors.end()) {
		mActors.erase(it); // リストから削除
		actor->OnDisable();
	}
}

size_t UIActorManager::GetActorIndex(UIActorObject* actor)
{
	auto iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end()) {
		size_t index = std::distance(mActors.begin(), iter);
		return index;
	}
	//未発見
	return -1;
}

UIActorObject* UIActorManager::FindActorByID(uint64_t id)
{
	return FindActorByIDInternal(mActors, id);
}

UIActorObject* UIActorManager::FindActorByIDInternal(const std::vector<UIActorObject*>& list, uint64_t id)
{
	for (auto actor : list) {
		if (actor->GetID() == id) return actor;

		// 子供のリストからも再帰的に探す
		auto& children = actor->GetRectTransform()->GetChildActorList();
		if (UIActorObject* found = FindActorByIDInternal(children, id)) {
			return found;
		}
	}
	return nullptr; // 見つからなかった場合
}
