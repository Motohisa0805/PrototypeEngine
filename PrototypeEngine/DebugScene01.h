#pragma once
#include "BaseScene.h"
#include "FPSCanvas.h"
#include "Stages00.h"

//�ʏ�̃��f���Ȃǂ𐶐����Ă���Q�[���V�[��
//FPS���_�̃Q�[���V�[��
//�Q�[���̏����������ł܂Ƃ߂čs���܂��B
//Unity��Scene�̂悤�Ȃ��́B
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