#pragma once
#include "BaseScene.h"
#include "FreeCameraActor.h"
#include "TitleCanvas.h"
#include "MeshActor.h"

//�^�C�g���V�[���̃N���X
class TitleScene : public BaseScene
{
private:
	MeshActor*						mDebugStage;

	class FreeCameraActor*			mFreeCameraActor;

	class TitleCanvas*				mTitleCanvas;
public:
									TitleScene();
									~TitleScene();

	bool							Initialize()override;

	bool							InputUpdate(const InputState& state)override;

	bool							Update()override;
};

