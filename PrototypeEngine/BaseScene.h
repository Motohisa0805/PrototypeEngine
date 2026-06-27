#pragma once
#include "GameWinMain.h"
#include "Typedefs.h"
#include "InputSystem.h"
#include "ActorManager.h"
#include "UIActorManager.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class ActorObject;
class UIActorObject;
class Canvas;
class Image;
class Font;
class Skeleton;
class BaseCamera;
class Text;

class AudioSystem;
//シーンの基底クラス
//オブジェクトの更新などをまとめて行う部分になります。
// シーンごとに継承して使用します。
class BaseScene
{
protected:
	//ActorObjectがコンストラクタで呼び出すための関数
	friend class ActorObject;
	friend class UIActorObject;

	AudioSystem*									mAudioSystem;
	
	ActorManager*									mActorManager;

	UIActorManager*									mUIActorManager;

	// Map for fonts
	std::unordered_map<string, Font*>				mFontMap;
	// Map of loaded skeletons
	std::unordered_map<string, Skeleton*>			mSkeletonMap;

	std::unordered_map<string, BaseCamera*>			mCameraMap;
	
	//Actorに割り当てるユニークなID/カウント
	int												mNextActorID;
	

	ActorObject*									mPlayer;

	// 50Hz、Unityと同じ
	const float										mFixed_Delta_Time = 0.02f;

	float											mFixedTimeAccumulator;


	//シーンの名前
	string											mName;
	//シーンが変更されたかどうかのフラグ
	bool											mIsDirtyFlag;

	//スカイボックスの画像パス保存変数
	string											mLoadSkyBoxTexturePath;
public:

	//コンストラクタ
													BaseScene();
	//初期化
	virtual bool									Initialize();
	//入力更新
	virtual bool									InputUpdate(const InputState& state);
	//固定更新
	virtual bool									FixedUpdate();
	//更新
	virtual bool									Update();
	//エディタ用更新
	virtual bool									EditorUpdate(bool isRun);
	void											ClearDirtyFlag();
	//解放
	void											UnloadData();

	void											LoadSkyBoxTexture(const string& file);

	string											GetLoadSkyBoxTexturePath() { return mLoadSkyBoxTexturePath; }

	void											SetLoadSkyBoxTexturePath(string path) { mLoadSkyBoxTexturePath = path; }

	ActorManager*									GetActorManager() { return mActorManager; }

	UIActorManager*									GetUIActorManager() { return mUIActorManager; }

	//FontのGetter
	Font*											GetFont(const string& fileName);
	//スケルトンのGetter
	Skeleton*										GetSkeleton(const string& fileName);
	//AudioSystemのGetter
	AudioSystem*									GetAudioSystem() { return mAudioSystem; }

	// Game-specific
	//PlayerオブジェクトのGetter
	ActorObject*									GetPlayer() { return mPlayer; }

	//Cameraの追加
	void											AddCamera(BaseCamera* camera);
	//Cameraの削除
	void											RemoveCamera(BaseCamera* camera);
	//Cameraの取得
	BaseCamera*										GetCamera(const string& name = "Camera0");

	std::unordered_map<string, BaseCamera*> 		GetCameras() { return mCameraMap; }

	//シーン名のGetter
	string											GetName() { return mName; }
	//シーン名のSetter
	virtual void									SetName(const string& name) { mName = name; }

	//シーンの変更フラグのGetter/Setter
	bool										    IsDirtyFlag() { return mIsDirtyFlag; }
	void										    SetDirtyFlag(bool isDirty) { mIsDirtyFlag = isDirty; }
	//シーン内のオブジェクトの頂点数を表示
	int												GetSceneAllVertices();
};

// ActorObjectリストを保持するEditorSceneクラスを仮定
class EditorScene : public BaseScene
{
private:

public:
	EditorScene();
	void EditorInitilaize();
	// ... BaseScene の仮想関数を実装 
	void SetName(const string& name)override;
};

#define Release_Function  0
#define Debug_Function  1