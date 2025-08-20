#include "DialogBox.h"

GameDialogBox::GameDialogBox(const string& text,
	std::function<void()> onOK)
	:Canvas()
{

	// ダイアログボックスの位置を調整する
	mBGPos = Vector2(0.0f, 0.0f);
	mTitlePos = Vector2(0.0f, 100.0f);
	mNextButtonPos = Vector2(0.0f, 0.0f);

	mBackground = new Image();
	mBackground->Load("DialogBG.png");

	mTitleFont = new Text(mGame->GetFont("NotoSansJP-Bold.ttf"), Vector2::Zero);
	mTitleFont->SetPosition(mTitlePos);
	SetTitle(text, Vector3::Zero, 30);
	CreateButton("Yes",Vector2(0.0f,0.0f), [onOK]() {
		onOK();
		});
	CreateButton("No",Vector2(0.0f, -80.0f), [this]() {
		Close();
		});
}

GameDialogBox::GameDialogBox(const char8_t* text, std::function<void()> onOK)
	:Canvas()
{
	// ダイアログボックスの位置を調整する
	mBGPos = Vector2(0.0f, 0.0f);
	mTitlePos = Vector2(0.0f, 100.0f);
	mNextButtonPos = Vector2(0.0f, 0.0f);

	mBackground = new Image();
	mBackground->Load("DialogBG.png");

	mTitleFont = new Text(mGame->GetFont("NotoSansJP-Bold.ttf"), Vector2::Zero);
	mTitleFont->SetPosition(mTitlePos);
	mTitleFont->SetColor(Vector3::Zero);
	mTitleFont->SetFontSize(30);
	mTitleFont->SetUTF_8Text(text);

	CreateButton(u8"はい", Vector2(0.0f, 0.0f), [onOK]() {
		onOK();
		});
	CreateButton(u8"いいえ", Vector2(0.0f, -80.0f), [this]() {
		Close();
		});
}

GameDialogBox::GameDialogBox(const char8_t* text, std::function<void()> onOK, std::function<void()> run)
	:Canvas()
{
	// ダイアログボックスの位置を調整する
	mBGPos = Vector2(0.0f, 0.0f);
	mTitlePos = Vector2(0.0f, 100.0f);
	mNextButtonPos = Vector2(0.0f, 0.0f);

	mBackground = new Image();
	mBackground->Load("DialogBG.png");

	mTitleFont = new Text(mGame->GetFont("NotoSansJP-Bold.ttf"), Vector2::Zero);
	mTitleFont->SetPosition(mTitlePos);
	mTitleFont->SetColor(Vector3::Zero);
	mTitleFont->SetFontSize(30);
	mTitleFont->SetUTF_8Text(text);

	CreateButton(u8"はい", Vector2(0.0f, 0.0f), [onOK]() {
		onOK();
		});
	CreateButton(u8"いいえ", Vector2(0.0f, -80.0f), [this]() {
		Close();
		});
	run();
}

GameDialogBox::GameDialogBox(const char8_t* text, std::function<void()> onOK, std::function<void()> run, std::function<void()> exitRun)
	:Canvas()
{
	// ダイアログボックスの位置を調整する
	mBGPos = Vector2(0.0f, 0.0f);
	mTitlePos = Vector2(0.0f, 100.0f);
	mNextButtonPos = Vector2(0.0f, 0.0f);

	mBackground = new Image();
	mBackground->Load("DialogBG.png");

	mTitleFont = new Text(mGame->GetFont("NotoSansJP-Bold.ttf"), Vector2::Zero);
	mTitleFont->SetPosition(mTitlePos);
	mTitleFont->SetColor(Vector3::Zero);
	mTitleFont->SetFontSize(30);
	mTitleFont->SetUTF_8Text(text);

	CreateButton(u8"はい", Vector2(0.0f, 0.0f), [onOK]() {
		onOK();
		});
	CreateButton(u8"いいえ", Vector2(0.0f, -80.0f), [this]() {
		Close();
		});
	run();
	mExitRun = exitRun;
}

GameDialogBox::GameDialogBox(const char8_t* text, std::function<void()> onOK, std::function<void()> onNO, std::function<void()> exitRun, const bool onOffFrag)
	:Canvas()
{
	// ダイアログボックスの位置を調整する
	mBGPos = Vector2(0.0f, 0.0f);
	mTitlePos = Vector2(0.0f, 100.0f);
	mNextButtonPos = Vector2(0.0f, 0.0f);

	mBackground = new Image();
	mBackground->Load("DialogBG.png");

	mTitleFont = new Text(mGame->GetFont("NotoSansJP-Bold.ttf"), Vector2::Zero);
	mTitleFont->SetPosition(mTitlePos);
	mTitleFont->SetColor(Vector3::Zero);
	mTitleFont->SetFontSize(30);
	mTitleFont->SetUTF_8Text(text);

	CreateButton(u8"はい", Vector2(0.0f, 0.0f), [onOK]() {
		onOK();
		});
	CreateButton(u8"いいえ", Vector2(0.0f, -80.0f), [this]() {
		Close();
		});
	mOnNoRun = onNO;
	mExitRun = exitRun;
}


GameDialogBox::~GameDialogBox()
{
	if(mExitRun)
	{
		mExitRun();
	}
}

void GameDialogBox::Close()
{
	Canvas::Close();
	if (mOnNoRun)
	{
		mOnNoRun();
	}
}
