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
	//アクターの状態を更新します（EDeadになったものは後で削除）
	for (int i = 0; i < mActors.size(); i++)
	{
		mActors[i]->StateUpdate(time);
	}
	// 死亡アクターを一時ベクターに追加する
	vector<ActorObject*> deadActors;
	for (int i = 0; i < mActors.size(); i++)
	{
		if (mActors[i]->GetState() == ActorObject::EDead)
		{
			deadActors.emplace_back(mActors[i]);
		}
	}


	// 死んだアクターを削除する
	for (auto actor : deadActors)
	{
		actor->OnDestroy();
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
	//全てのアクターを順番に破棄
	for (auto actor : mActors)
	{
		// デストラクタ内で Manager を呼ばないため、ここで安全に delete できる
		delete actor;
	}

	//リストを空にする
	mActors.clear();
}

void ActorManager::AddActor(ActorObject* actor)
{
	if (mUpdatingActors)
	{
		// もしアクターを更新する場合、保留中に追加する必要あり
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
	// mPendingActorsに指定したアクターがあるか？
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		// ベクターの末尾にスワップしてポップする（eraseのコピーを避ける）
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	// mActorsに指定したアクターがあるか？
	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// ベクターの末尾にスワップしてポップする（eraseのコピーを避ける）
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

void ActorManager::DeleteActor(ActorObject* actor)
{
	actor->SetState(ActorObject::EDead);
}

void ActorManager::ReAddActor(ActorObject* actor)
{
	if (mUpdatingActors)
	{
		// もしアクターを更新している場合は、保留中に追加する必要あり
		mPendingActors.emplace_back(actor);
	}
	else
	{
		// 更新中でない場合はメインリストに直接追加（エディタ操作は通常こちら）
		mActors.push_back(actor);
		actor->OnEnabled();
	}
}

void ActorManager::DetachActor(ActorObject* actor)
{
	auto it = std::find(mActors.begin(), mActors.end(), actor);
	if (it != mActors.end()) {
		mActors.erase(it); // リストから削除
		actor->OnDisable();
	}
}

size_t ActorManager::GetActorIndex(ActorObject* actor)
{
	auto iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end()) {
		size_t index = std::distance(mActors.begin(), iter);
		return index;
	}
	//未発見
	return -1;
}

ActorObject* ActorManager::FindActorByID(uint64_t id)
{
	return FindActorByIDInternal(mActors, id);
}

ActorObject* ActorManager::FindActorByIDInternal(const std::vector<ActorObject*>& list, uint64_t id)
{
	for (auto actor : list) {
		if (actor->GetID() == id) return actor;

		// 子供のリストからも再帰的に探す
		auto& children = actor->GetTransform()->GetChildActorList();
		if (ActorObject* found = FindActorByIDInternal(children, id)) {
			return found;
		}
	}
	return nullptr; // 見つからなかった場合
}
