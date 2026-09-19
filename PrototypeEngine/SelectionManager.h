#pragma once
#include "GUIEditorManager.h"
#include "Entity.h"
#include "AnimatorControllerParameters.h"

//選択オブジェクトの管理クラス
class SelectionManager
{
private:
	//選択中のアクターを保持するポインター
	static Entity*			mSelectedActor;

	static filesystem::path mSelectedFilePath;

	static string			mSelectedStateName;

public:
	static void		AllClear()
	{
		mSelectedActor    = nullptr;
        mSelectedFilePath = "Assets";
        mSelectedStateName.clear();
	}

	static Entity*	GetSelectedActor() { return mSelectedActor; }
    static void		SetSelectedActor(Entity* actor);

	static filesystem::path GetSelectedFilePath() { return mSelectedFilePath; }
	static void		SetSelectedFilePath(const filesystem::path& path) 
	{
		mSelectedFilePath = path; 
		mSelectedActor    = nullptr;
        mSelectedStateName.clear();
	}

	static string	GetSelectedStateName() { return mSelectedStateName; }
    static void		SetSelectedStateName(string name) 
	{
		mSelectedStateName = name;
        mSelectedActor     = nullptr;
        mSelectedFilePath  = "Assets";
	}
    static void		ClearStateSelection() { mSelectedStateName.clear(); }
};

