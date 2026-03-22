#pragma once
#include "Typedefs.h"

//前方宣言
class ActorObject;

class ActorManager
{
private:
	//保留用の配列
	vector<ActorObject*>			mPendingActors;
	//更新用の配列
	vector<ActorObject*>			mActors;

	bool							mUpdatingActors;
public:
	void							UpdateActors(float time);
	void							FixedUpdateActors(float time);

	void							UnloadActors();
	const vector<ActorObject*>&		GetActors()const { return mActors; }
	//オブジェクト追加
	void							AddActor(ActorObject* actor);
	//オブジェクト削除
	void							RemoveActor(ActorObject* actor);
	void							DeleteActor(ActorObject* actor);
};
