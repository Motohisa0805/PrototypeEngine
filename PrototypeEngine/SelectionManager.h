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

	static string			mSelectedTransitionName;

public:
	static void		AllClear()
	{
		mSelectedActor    = nullptr;
        mSelectedFilePath = "Assets";
        mSelectedStateName.clear();
        mSelectedTransitionName.clear();
	}

	static Entity*	GetSelectedActor() { return mSelectedActor; }
    static void		SetSelectedActor(Entity* actor);

	static filesystem::path GetSelectedFilePath() { return mSelectedFilePath; }
	static void		SetSelectedFilePath(const filesystem::path& path) 
	{
		mSelectedFilePath = path; 
		mSelectedActor    = nullptr;
        mSelectedStateName.clear();
        mSelectedTransitionName.clear();
	}

	static string	GetSelectedStateName() { return mSelectedStateName; }
    static void		SetSelectedStateName(string name) 
	{
		mSelectedStateName = name;
        mSelectedTransitionName.clear();
        mSelectedActor     = nullptr;
        mSelectedFilePath  = "Assets";
	}
    static void		ClearStateSelection() { mSelectedStateName.clear(); }

	static string GetSelectedTransitionName() {return mSelectedTransitionName;}
	static void SetSelectedTransitionName(string name)
	{
        mSelectedTransitionName = name;
        mSelectedStateName.clear();
        mSelectedActor          = nullptr;
        mSelectedFilePath       = "Assets";
	}
    static void ClearTransitionSelection() { mSelectedTransitionName.clear(); }
};

