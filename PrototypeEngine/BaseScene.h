#pragma once
#include "GameWinMain.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//シーンの基底クラス
//オブジェクトの更新などをまとめて行う部分になります。
// シーンごとに継承して使用します。
class BaseScene
{
public:
	enum MouseMode
	{
		Relative, // 相対モード
		Absolute  // 絶対モード
	};
protected:

	AudioSystem*									mAudioSystem;

	PhysWorld*										mPhysWorld;
	// Track if we're updating actors right now
	bool											mUpdatingActors;

	// All the actors in the game
	vector<class ActorObject*>						mActors;
	vector<class Canvas*>							mCanvasStack;
	vector<class Image*>							mImageStack;
	vector<class Image*>							mDebugImageStack;
	// Map for fonts
	std::unordered_map<string, class Font*>			mFonts;
	// Map of loaded skeletons
	std::unordered_map<string, class Skeleton*>		mSkeletons;
	// Any pending actors
	vector<class ActorObject*>						mPendingActors;


	class ActorObject*								mPlayer;

	class DirectionalLightActor*					mDirectionalLightActor;

	class BaseCamera*								mMainCamera;

	// 50Hz、Unityと同じ
	const float										mFixed_Delta_Time = 0.02f;

	float											mFixedTimeAccumulator;

	//***デバッグ機能***
	//フレームレート表示テキスト
	class Text*										mFrameRateText;
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
	//解放
	void											UnloadData();

	void											SetMouseMode(MouseMode mode);

	void											LoadSkyBoxTexture(string file);

	vector<class ActorObject*>&						GetActors() { return mActors; }
	//オブジェクト追加
	void											AddActor(class ActorObject* actor);
	//オブジェクト削除
	void											RemoveActor(class ActorObject* actor);

	template<typename T>
	vector<ActorObject*>							SelectAllActorComponent();

	//FontのGetter
	class Font*										GetFont(const string& fileName);
	//スケルトンのGetter
	class Skeleton*									GetSkeleton(const string& fileName);
	//AudioSystemのGetter
	class AudioSystem*								GetAudioSystem() { return mAudioSystem; }
	//PhysWorldのGetter
	class PhysWorld*								GetPhysWorld() { return mPhysWorld; }

	// Manage UI stack
	const vector<class Canvas*>&					GetUIStack() { return mCanvasStack; }
	//UIScreenの設定
	void											PushUI(class Canvas* screen);
	//Image配列のGetter
	const vector<class Image*>&						GetImageStack() { return mImageStack; }
	const vector<class Image*>&						GetDebugImageStack() { return mDebugImageStack; }
	//Imageの追加
	void											PushImage(class Image* screen);
	void											RemoveImage(class Image* screen);
	//DebugImageの追加
	void											PushDebugImage(class Image* screen);
	void											RemoveDebugImage(class Image* screen);
	// Game-specific
	//PlayerオブジェクトのGetter
	class ActorObject*								GetPlayer() { return mPlayer; }
	//環境光の設定
	class DirectionalLightActor*					GetDirectionalLightActor() { return mDirectionalLightActor; }
	//CameraのGetter
	class BaseCamera*								GetCamera() { return mMainCamera; }
	//MainCameraの設定
	void											SetMainCamera(class BaseCamera* camera) { mMainCamera = camera; }
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

#define Release_Function  0
#define Debug_Function  1