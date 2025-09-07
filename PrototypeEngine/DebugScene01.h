#pragma once
#include "BaseScene.h"
#include "FPSCanvas.h"
#include "Stages00.h"

//前方宣言	
class FPSActor;
class PauseMenu;

//通常のモデルなどを生成しているゲームシーン
//FPS視点のゲームシーン
//ゲームの処理をここでまとめて行います。
//UnityのSceneのようなもの。
class DebugScene01 : public BaseScene
{
private:
	// Game-specific code
	FPSActor*						mFPSActor;

	Stages00*						mStages;

	SoundEventClip					mMusicEvent;

	FPSCanvas*						mGameCanvas00;

	PauseMenu*						mPauseMenu;
public:
									DebugScene01();

	bool							Initialize()override;

	bool							InputUpdate(const InputState& state)override;

	bool							Update()override;
};