#pragma once
#include "GUIEditorManager.h"
#include "Entity.h"

//選択オブジェクトの管理クラス
class SelectionManager
{
private:
	//選択中のアクターを保持するポインター
	static Entity* mSelectedActor;
public:
	static Entity* GetSelectedActor() { return mSelectedActor; }
	static void SetSelectedActor(Entity* actor) { mSelectedActor = actor; }
};

