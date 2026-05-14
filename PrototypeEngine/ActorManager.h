#pragma once
#include "Typedefs.h"

//前方宣言
class ActorObject;
//Actorオブジェクトの管理クラス
//追加、削除、更新、解放を行う
class ActorManager
{
private:
	//保留用の配列
	vector<ActorObject*>			mPendingActors;
	//更新用の配列
	vector<ActorObject*>			mActors;
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
	const vector<ActorObject*>&		GetActors()const { return mActors; }
	vector<ActorObject*>&			GetActorsMutable() { return mActors; }
	//Actor追加
	void							AddActor(ActorObject* actor);
	//Actor解放
	void							RemoveActor(ActorObject* actor);
	//Actorの削除
	void							DeleteActor(ActorObject* actor);
	void							ReAddActor(ActorObject* actor);
	//Actorを配列から外す
	void							DetachActor(ActorObject* actor);

	//ActorのIndexを検索
	size_t							GetActorIndex(ActorObject* actor);
};
