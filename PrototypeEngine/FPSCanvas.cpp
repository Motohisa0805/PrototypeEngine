#include "FPSCanvas.h"
#include "PauseMenu.h"

FPSCanvas::FPSCanvas()
	:Canvas()
	, mRadarRange(2000.0f)
	, mRadarRadius(92.0f)
	, mTargetEnemy(false)
	, mCrosshairAngle(0)
{
	Renderer* r = EngineWindow::GetRenderer();

	Font* font = mGame->GetFont("NotoSansJP-Bold.ttf");

	mSceneNameFrame = new Image();
	mSceneNameFrame->Load("UIFrame.png");
	mSceneNameFrame->SetPosition(Vector2(500, 350));
	mSceneNameFrame->SetScale(Vector3(1.25f, 1.0f, 1.0f));
	//シーン名生成
	mSceneNameText = new Text(font, Vector2(500, 350));
	mSceneNameText->SetUTF_8Text(u8"FPS視点シーン");
	mSceneNameText->SetFontSize(40);


	mPoseButtonFrame = new Image();
	mPoseButtonFrame->Load("ButtonUI.png");
	mPoseButtonFrame->SetPosition(Vector2(-550, 330));

	mPoseButton = new Image();
	mPoseButton->Load("ESCUI.png");
	mPoseButton->SetPosition(Vector2(-550, 350));
	mPoseButton->SetScale(Vector3(0.5f, 0.5f, 0.5f));
	mPoseButtonText = new Text(font, Vector2(-550, 310));
	mPoseButtonText->SetUTF_8Text(u8"ポーズ");
	mPoseButtonText->SetFontSize(40);

	mRadar = new Image();
	mRadar->Load("Radar.png");

	mCrosshair = new Image();
	mCrosshair->Load("Crosshair.png");
	mCrosshair->SetFillMethod(Image::Radial360);

	mCrosshairEnemy = new Image();
	mCrosshairEnemy->Load("CrosshairRed.png");
	
	mBlipTex = new Image();
	mBlipTex->Load("Blip.png");
	mBlipTex->SetState(Image::EClosing);

	mRadarArrow = new Image();
	mRadarArrow->Load("RadarArrow.png");


	mHelthBar = new Image();
	mHelthBar->Load("HelthBar.png");

	mHelthBarFrame = new Image();
	mHelthBarFrame->Load("HelthBarFrame.png");


	mHelthBar->SetFillMethod(Image::Horizontal);

	mHelthBarFrame->SetPosition(Vector2(-300,-300));
	mHelthBar->SetPosition(Vector2(-300,-300));

	mTargetComponentSystem = new TargetComponentSystem();
	mTargetComponentSystem->AllTargetCheck();
}

FPSCanvas::~FPSCanvas()
{
}

void FPSCanvas::Update(float deltaTime)
{
	Canvas::Update(deltaTime);

	UpdateBlipTextures();

	UpdateCrosshair(deltaTime);
	UpdateRadar(deltaTime);

	//各画像の座標更新

	// Crosshair
	if (GameStateClass::mGameState == TimeStop) { return; }
	if (mTargetEnemy)
	{
		mCrosshair->SetState(Image::EClosing);
		mCrosshairEnemy->SetState(Image::EActive);
		mCrosshairEnemy->SetPosition(Vector2::Zero);
	}
	else
	{
		mCrosshairEnemy->SetState(Image::EClosing);
		mCrosshair->SetState(Image::EActive);
		mCrosshair->SetPosition(Vector2::Zero);
	}
	//Image* crosshair = mTargetEnemy ? mCrosshairEnemy : mCrosshair;
	//mCrosshairAngle++;
	//crosshair->SetAngleZ(mCrosshairAngle);
	//crosshair->SetScale(Vector3(1.5f, 0.5f,1.0f));


	// Radar
	const Vector2 cRadarPos(-390.0f, 275.0f);
	mRadar->SetPosition(cRadarPos);
	// Blips
	/*
	for (Vector2& blip : mBlips)
	{
		blip *= 100.0f;
		mBlipTex->SetPosition(cRadarPos + blip);
	}
	*/
	for(int i = 0; i < mBlips.size(); i++)
	{
		Vector2 blip = mBlips[i];
		blip *= 100.0f;
		Vector2 blipPos = cRadarPos + blip;
		mBlipTexs[i]->SetPosition(blipPos);
		/*
		if (i < mBlipTexs.size())
		{
			mBlipTexs[i]->SetPosition(cRadarPos + mBlips[i] * 100.0f);
			mBlipTexs[i]->SetState(Image::EActive);
		}
		else
		{
			Image* blipTex = new Image(*mBlipTex);
			blipTex->SetPosition(cRadarPos + mBlips[i] * 100.0f);
			mBlipTexs.push_back(blipTex);
		}
		*/
	}

	// Radar arrow
	mRadarArrow->SetPosition(cRadarPos);
}

void FPSCanvas::ProcessInput(const InputState& keys)
{
	//テスト用入力
	if (keys.Keyboard.GetKey(KEY_DOWN))
	{
		float fill = mCrosshair->GetFillAmount();
		fill -= 0.01f;
		if (fill <= 0)
		{
			fill = 0;
		}
		mCrosshair->SetFillAmount(fill);
	}
	else if (keys.Keyboard.GetKey(KEY_UP))
	{
		float fill = mCrosshair->GetFillAmount();
		fill += 0.01f;
		if (fill >= 1)
		{
			fill = 1;
		}
		mCrosshair->SetFillAmount(fill);
	}

	if (keys.Keyboard.GetKeyDown(KEY_ESCAPE))
	{
		new PauseMenu();
	}
}

void FPSCanvas::UpdateCrosshair(float deltaTime)
{
	// Reset to regular cursor
	mTargetEnemy = false;
	// Make a line segment
	const float cAimDist = 500.0f;
	Vector3 start = Vector3::Zero;
	Vector3 dir = Vector3::Zero;
	EngineWindow::GetRenderer()->GetScreenDirection(start, dir);
	LineSegment l(start, start + dir * cAimDist);
	// Segment cast
	PhysWorld::CollisionInfo info;
	ActorTag tag = ActorTag::Enemy;
	if (mGame->GetPhysWorld()->RayCast(l, info, (int)tag))
	{
		// Is this a target?
		for (auto tc : mTargetComponentSystem->GetTargetComponent())
		{
			if (tc->GetOwner()->GetActorTag() == info.mActor->GetActorTag())
			{
				mTargetEnemy = true;
				break;
			}
		}
	}
}

void FPSCanvas::UpdateBlipTextures()
{
	int targetCount = static_cast<int>(mTargetComponentSystem->GetTargetComponent().size());
	int currentCount = static_cast<int>(mBlipTexs.size());

	if (targetCount == currentCount) return;

	// 増やす必要がある場合
	while (mBlipTexs.size() < targetCount)
	{
		Image* blipTex = new Image();
		blipTex->SetTexture(mBlipTex->GetTexture());
		mBlipTexs.emplace_back(blipTex); // mBlipTex は雛形？
	}

	// 減らす必要がある場合
	while (mBlipTexs.size() > targetCount)
	{
		if (!mBlipTexs.empty())
		{
			auto* tex = mBlipTexs.back();
			if (tex) tex->SetState(Image::EDestroy);
			mBlipTexs.pop_back();
		}
	}
}

void FPSCanvas::UpdateRadar(float deltaTime)
{
	// Clear blip positions from last frame
	mBlips.clear();

	// Convert player position to radar coordinates (x forward, z up)
	Vector3 playerPos = mGame->GetPlayer()->GetLocalPosition();
	Vector2 playerPos2D(playerPos.x, playerPos.z);
	// Ditto for player forward
	Vector3 playerForward = mGame->GetPlayer()->GetForward();
	Vector2 playerForward2D(playerForward.z, playerForward.x);

	// Use atan2 to get rotation of radar
	float angle = Math::Atan2(playerForward2D.y, playerForward2D.x);
	// Make a 2D rotation matrix
	Matrix3 rotMat = Matrix3::CreateRotation(angle);

	// Get positions of blips
	for (auto tc : mTargetComponentSystem->GetTargetComponent())
	{
		Vector3 targetPos = tc->GetOwner()->GetLocalPosition();
		Vector2 actorPos2D(targetPos.x, targetPos.z);

		// Calculate vector between player and target
		Vector2 playerToTarget = actorPos2D - playerPos2D;

		// See if within range
		if (playerToTarget.LengthSq() <= (mRadarRange * mRadarRange))
		{
			// Convert playerToTarget into an offset from
			// the center of the on-screen radar
			Vector2 blipPos = playerToTarget;
			blipPos *= mRadarRadius / mRadarRange;

			// Rotate blipPos
			blipPos = Vector2::Transform(blipPos, rotMat);
			mBlips.emplace_back(blipPos);
		}
	}
}