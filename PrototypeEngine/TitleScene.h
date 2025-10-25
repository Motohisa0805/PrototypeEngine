#pragma once
#include "BaseScene.h"


//エンジンでは派生クラスとして拡張性がなくなるの廃止予定

class FreeCameraActor;
class MeshActor;
class TitleCanvas;

//タイトルシーンのクラス
class TitleScene : public BaseScene
{
private:
	MeshActor*						mDebugStage;

	FreeCameraActor*				mFreeCameraActor;

	TitleCanvas*					mTitleCanvas;
public:
									TitleScene();
									~TitleScene();

	bool							Initialize()override;

	bool							InputUpdate(const InputState& state)override;

	bool							Update()override;
};

