#include "DebugScene02.h"
#include "GameFunctions.h"

DebugScene02::DebugScene02()
	: mTPSPlayer(nullptr)
	, mTPSCanvas(nullptr)
	, mStages01(nullptr)
{
}

bool DebugScene02::Initialize()
{
	BaseScene::Initialize();

	// ポイントライトメッシュをロードする
	//mWinMain->GetRenderer()->SetPointLightMesh(mWinMain->GetRenderer()->GetMesh("PointLight.gpmesh"));
	

	// Start music
	mMusicEvent = mAudioSystem->PlayEvent("event:/Music");

	InputSystem::SetGameMouseMode(InputSystem::RelativeMouse);

	// デバッグ用のステージ追加
	mStages01 = new Stages01();
	mStages01->Initialize();

	mTPSPlayer = new TPSPlayer();
	mTPSPlayer->SetLocalPosition(Vector3(0.0f, 0.5f, 0.0f));

	// プレイヤー生成
	mPlayer = mTPSPlayer;

	// ゲーム内のUI生成
	mTPSCanvas = new TPSCanvas();

	LoadSkyBoxTexture("SkyBox03.png");

	return true;
}

bool DebugScene02::InputUpdate(const InputState& state)
{
	BaseScene::InputUpdate(state);

	if (!mCanvasStack.empty())
	{
		mCanvasStack.back()->ProcessInput(state);
	}

	return true;
}

bool DebugScene02::Update()
{
	BaseScene::Update();
	return true;
}