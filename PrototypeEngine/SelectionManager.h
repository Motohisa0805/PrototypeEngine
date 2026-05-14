#pragma once
#include "GUIWinMain.h"
#include "Actor.h"

//選択オブジェクトの管理クラス
class SelectionManager
{
private:
	//選択中のアクターを保持するポインター
	static ActorObject* mSelectedActor;
public:
	static ActorObject* GetSelectedActor() { return mSelectedActor; }
	static void SetSelectedActor(ActorObject* actor) { mSelectedActor = actor; }
};

