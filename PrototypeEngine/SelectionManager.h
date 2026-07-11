#pragma once
#include "GUIEditorManager.h"
#include "Entity.h"

//選択オブジェクトの管理クラス
class SelectionManager
{
private:
	//選択中のアクターを保持するポインター
	static Entity*			mSelectedActor;

	static filesystem::path mSelectedFilePath;

public:
	static Entity* GetSelectedActor() { return mSelectedActor; }
	static void SetSelectedActor(Entity* actor) { mSelectedActor = actor; }

	static filesystem::path GetSelectedFilePath() { return mSelectedFilePath; }
	static void SetSelectedFilePath(const filesystem::path& path) { mSelectedFilePath = path; }
};

