#pragma once
#include "BaseScene.h"
#include "SoundEventClip.h"
#include "TPSCanvas.h"
#include "Stages01.h"

//TPS視点のゲームシーン
//ゲームの処理をここでまとめて行います。
// UnityのSceneのようなもの。
class DebugScene02 : public BaseScene
{
private:
	// Game-specific code

	class TPSPlayer*				mTPSPlayer;

	Stages01*						mStages01;

	SoundEventClip					mMusicEvent;

	TPSCanvas*						mTPSCanvas;
public:
									DebugScene02();
	
	bool							Initialize()override;
	
	bool							InputUpdate(const InputState& state)override;

	bool							Update()override;
};