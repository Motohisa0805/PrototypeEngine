#pragma once
#include "BaseScene.h"
#include "SoundEventClip.h"
#include "TPSCanvas.h"
#include "Stages01.h"

class TPSPlayer;
//TPS���_�̃Q�[���V�[��
//�Q�[���̏����������ł܂Ƃ߂čs���܂��B
// Unity��Scene�̂悤�Ȃ��́B
class DebugScene02 : public BaseScene
{
private:
	// Game-specific code

	TPSPlayer*						mTPSPlayer;

	Stages01*						mStages01;

	SoundEventClip					mMusicEvent;

	TPSCanvas*						mTPSCanvas;
public:
									DebugScene02();
	
	bool							Initialize()override;
	
	bool							InputUpdate(const InputState& state)override;

	bool							Update()override;
};