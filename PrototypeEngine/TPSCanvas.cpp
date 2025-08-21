#include "TPSCanvas.h"
#include "PauseMenu.h"

TPSCanvas::TPSCanvas()
	: Canvas()
{
	Renderer* r = EngineWindow::GetRenderer();

	Font* font = mGame->GetFont("NotoSansJP-Bold.ttf");
	//シーン名生成
	mSceneNameFrame = new Image();
	mSceneNameFrame->Load("UIFrame.png");
	mSceneNameFrame->SetPosition(Vector2(500, 350));
	mSceneNameFrame->SetScale(Vector3(1.25f, 1.0f, 1.0f));
	//シーン名生成
	mSceneNameText = new Text(font, Vector2(500, 350));
	mSceneNameText->SetUTF_8Text(u8"TPS視点シーン");
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

	mPoseFrame = new Image();
	mPoseFrame->Load("UIFrame.png");
	mPoseFrame->SetPosition(Vector2(0.0f,-360));
	mPoseFrame->SetScale(Vector3(8.0f,1.5f,1.0f));

	mTPoseButtonText = new Text(font, Vector2(-500, -350));
	mTPoseButtonText->SetUTF_8Text(u8"1:Tポーズ");
	mTPoseButtonText->SetFontSize(40);

	mIdlePoseButtonText = new Text(font, Vector2(-300, -350));
	mIdlePoseButtonText->SetUTF_8Text(u8"2:待機");
	mIdlePoseButtonText->SetFontSize(40);

	mRunPoseButtonText = new Text(font, Vector2(-100, -350));
	mRunPoseButtonText->SetUTF_8Text(u8"3:走る");
	mRunPoseButtonText->SetFontSize(40);

	mJumpPoseButtonText = new Text(font, Vector2(100, -350));
	mJumpPoseButtonText->SetUTF_8Text(u8"4:ジャンプ");
	mJumpPoseButtonText->SetFontSize(40);

	mCapoeiraPoseButtonText = new Text(font, Vector2(300, -350));
	mCapoeiraPoseButtonText->SetUTF_8Text(u8"5:ダンス");
	mCapoeiraPoseButtonText->SetFontSize(40);

	mHelthBar = new Image();
	mHelthBar->Load("HelthBar.png");
	mHelthBarFrame = new Image();
	mHelthBarFrame->Load("HelthBarFrame.png");
	mHelthBar->SetFillMethod(Image::Horizontal);
	mHelthBarFrame->SetPosition(Vector2(-300, -300));
	mHelthBar->SetPosition(Vector2(-300, -300));
}

void TPSCanvas::Update(float deltaTime)
{
	Canvas::Update(deltaTime);
}

void TPSCanvas::ProcessInput(const InputState& keys)
{
	Canvas::ProcessInput(keys);
	if (keys.Keyboard.GetKeyDown(KEY_ESCAPE))
	{
		new PauseMenu();
	}
}
