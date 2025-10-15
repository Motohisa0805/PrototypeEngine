#pragma once
#include "GameWinMain.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class ActorObject;
class Canvas;
class Image;
class Font;
class Skeleton;
class DirectionalLightActor;
class BaseCamera;
class DirectionalLightComponent; //仮追加
class FreeCamera;               // 仮追加
class Text;
//シーンの基底クラス
//オブジェクトの更新などをまとめて行う部分になります。
// シーンごとに継承して使用します。
class BaseScene
{
protected:
	//ActorObjectがコンストラクタで呼び出すための関数
	friend class ActorObject;
	// Track if we're updating actors right now
	bool											mUpdatingActors;

	AudioSystem*									mAudioSystem;

	PhysWorld*										mPhysWorld;

	// Any pending actors
	vector<ActorObject*>							mPendingActors;

	// All the actors in the game
	vector<ActorObject*>							mActors;
	
	
	vector<Canvas*>									mCanvasStack;
	
	vector<Image*>									mImageStack;
	
	vector<Image*>									mDebugImageStack;

	// Map for fonts
	std::unordered_map<string, Font*>				mFonts;
	// Map of loaded skeletons
	std::unordered_map<string, Skeleton*>			mSkeletons;

	std::unordered_map<string, BaseCamera*>			mCameras;
	
	//Actorに割り当てるユニークなID/カウント
	int												mNextActorID;
	

	ActorObject*									mPlayer;

	// 50Hz、Unityと同じ
	const float										mFixed_Delta_Time = 0.02f;

	float											mFixedTimeAccumulator;
	//***デバッグ機能***
	//フレームレート表示テキスト
	Text*											mFrameRateText;


	//シーンの名前
	string											mName;
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
	virtual bool									EditorUpdate();
	//解放
	void											UnloadData();

	void											LoadSkyBoxTexture(string file);

	const vector<ActorObject*>&						GetActors()const { return mActors; }
	//オブジェクト追加
	void											AddActor(ActorObject* actor);
	//オブジェクト削除
	void											RemoveActor(ActorObject* actor);
	void											DeleteActor(ActorObject* actor);
	//更新待ちのオブジェクトをメインリストに移動する関数
	void											ProcessPendingActors();

	template<typename T>
	vector<ActorObject*>							SelectAllActorComponent();

	//FontのGetter
	Font*											GetFont(const string& fileName);
	//スケルトンのGetter
	Skeleton*										GetSkeleton(const string& fileName);
	//AudioSystemのGetter
	AudioSystem*									GetAudioSystem() { return mAudioSystem; }
	//PhysWorldのGetter
	PhysWorld*										GetPhysWorld() { return mPhysWorld; }

	// Manage UI stack
	const vector<Canvas*>&							GetUIStack() { return mCanvasStack; }
	//UIScreenの設定
	void											PushUI(Canvas* screen);
	//Image配列のGetter
	const vector<Image*>&							GetImageStack() { return mImageStack; }
	const vector<Image*>&							GetDebugImageStack() { return mDebugImageStack; }
	//Imageの追加
	void											PushImage(Image* screen);
	void											RemoveImage(Image* screen);
	//DebugImageの追加
	void											PushDebugImage(Image* screen);
	void											RemoveDebugImage(Image* screen);
	// Game-specific
	//PlayerオブジェクトのGetter
	ActorObject*									GetPlayer() { return mPlayer; }

	//Cameraの追加
	void											AddCamera(BaseCamera* camera);
	//Cameraの削除
	void											RemoveCamera(BaseCamera* camera);
	//Cameraの取得
	BaseCamera*										GetCamera(const string& name = "Camera0");

	std::unordered_map<string, BaseCamera*> 		GetCameras() { return mCameras; }

	//シーン内のアクティブな方向性ライトコンポーネントを取得
	DirectionalLightComponent*						GetActiveDirectionalLightComponent();

	//シーン内のメインカメラコンポーネントを取得
	BaseScene*										GetMainCameraComponent();

	//シーン名のGetter
	string											GetName() { return mName; }
	//シーン名のSetter
	virtual void									SetName(const string& name) { mName = name; }

	bool										    mIsComputeWorldTransform;
};

template<typename T>
inline vector<ActorObject*> BaseScene::SelectAllActorComponent()
{
	std::vector<ActorObject*> result;

	for (auto* actor : mActors)
	{
		for (auto* component : actor->GetComponents())
		{
			if (dynamic_cast<T*>(component)) // T型のComponentがあるか
			{
				result.push_back(actor);
				break; // 一つでも見つかればそのオブジェクトは対象になる
			}
		}
	}

	return result;
}

// ActorObjectリストを保持するEditorSceneクラスを仮定
class EditorScene : public BaseScene
{
public:

	void EditorInitilaize();
	// ... BaseScene の仮想関数を実装 
	void SetName(const string& name)override;
};

#define Release_Function  0
#define Debug_Function  1