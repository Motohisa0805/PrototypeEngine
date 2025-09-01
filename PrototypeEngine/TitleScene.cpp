#include "TitleScene.h"

TitleScene::TitleScene()
	: mDebugStage(nullptr)
	, mFreeCameraActor(nullptr)
	, mTitleCanvas(nullptr)
{
}

TitleScene::~TitleScene()
{
}

bool TitleScene::Initialize()
{
	BaseScene::Initialize();


	InputSystem::SetMouseMode(InputSystem::AbsoluteMouse);

	mTitleCanvas = new TitleCanvas();

	mFreeCameraActor = new FreeCameraActor();
	mFreeCameraActor->SetLocalPosition(Vector3(0.0f, 0.0f, 0.0f));
	mFreeCameraActor->GetCamera()->SetCameraPosition(mFreeCameraActor->GetPosition(), Vector3::UnitZ);

	LoadSkyBoxTexture("SkyBox02.png");

	return true;
}

bool TitleScene::InputUpdate(const InputState& state)
{
	if (!mCanvasStack.empty())
	{
		mCanvasStack.back()->ProcessInput(state);
	}
	BaseScene::InputUpdate(state);
	return true;
}

bool TitleScene::Update()
{
	BaseScene::Update();
	return true;
}
