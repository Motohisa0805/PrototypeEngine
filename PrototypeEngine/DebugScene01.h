#pragma once
#include "BaseScene.h"
#include "FPSCanvas.h"
#include "Stages00.h"

//通常のモデルなどを生成しているゲームシーン
//FPS視点のゲームシーン
//ゲームの処理をここでまとめて行います。
//UnityのSceneのようなもの。
class DebugScene01 : public BaseScene
{
private:
	// Game-specific code
	class FPSActor*					mFPSActor;

	Stages00*						mStages;

	class SoundEventClip			mMusicEvent;

	FPSCanvas*						mGameCanvas00;

	class PauseMenu*				mPauseMenu;
public:
									DebugScene01();

	bool							Initialize()override;

	bool							InputUpdate(const InputState& state)override;

	bool							Update()override;
};