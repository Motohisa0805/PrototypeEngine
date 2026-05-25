#pragma once
#include "Typedefs.h"


//前方宣言
class UIActorObject;

class UIActorManager
{
private:
	//保留用の配列
	vector<UIActorObject*>			mPendingActors;
	//更新用の配列
	vector<UIActorObject*>			mActors;
	//更新フラグ
	bool							mUpdatingActors;
public:
	//更新
	void							UpdateActors(float time);
	//固定更新
	void							FixedUpdateActors(float time);
	//解放
	void							UnloadActors();
	//ActorのGetter
	const vector<UIActorObject*>&	GetActors()const { return mActors; }
	vector<UIActorObject*>&			GetActorsMutable() { return mActors; }
	//Actor追加
	void							AddActor(UIActorObject* actor);
	//Actor解放
	void							RemoveActor(UIActorObject* actor);
	//Actorの削除
	void							DeleteActor(UIActorObject* actor);
	void							ReAddActor(UIActorObject* actor);
	//Actorを配列から外す
	void							DetachActor(UIActorObject* actor);

	//ActorのIndexを検索
	size_t							GetActorIndex(UIActorObject* actor);


	UIActorObject*					FindActorByID(uint64_t id);
	UIActorObject*					FindActorByIDInternal(const std::vector<UIActorObject*>& list, uint64_t id);
};

