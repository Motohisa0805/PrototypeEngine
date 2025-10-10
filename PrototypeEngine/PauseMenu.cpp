#include "PauseMenu.h"


PauseMenu::PauseMenu()
	:Canvas()
{
	GameStateClass::SetGameState(GameState::TimeStop);
	Time::gTimeScale = 0;
	InputSystem::SetGameMouseMode(InputSystem::AbsoluteMouse);

	mTitleImage = new Image();
	mTitleImage->Load("UIFrame.png");
	mTitleImage->SetPosition(Vector2(-400.0f, 200.0f));
	mTitleImage->SetScale(Vector3(1.5f, 1.5f, 1.0f));

	mTitleFont = new Text(mGame->GetFont("NotoSansJP-Bold.ttf"), Vector2::Zero);
	mTitlePos = Vector2(-400.0f, 200.0f);
	mTitleFont->SetPosition(mTitlePos);

	mTitleFont->SetColor(Color::White);
	mTitleFont->SetFontSize(40);
	mTitleFont->SetUTF_8Text(u8"ポーズ中");

	CreateButton(u8"再開",Vector2(-400.0f,100.0f), [this]() {
		Close();
		});
	//TODO : Load処理は一時的に無効
	/*
	CreateButton(u8"1つ前に戻る", Vector2(-400.0f, 20.0f), [this]() {
		int s = SceneManager::GetNowSceneIndex() == 1 ? 0 : 1;
		SceneManager::LoadSceneFromFile(s);
		});
	*/

	CreateButton(u8"終了", Vector2(-400.0f, -60.0f), [this]() {
		mDialogBox = new GameDialogBox(u8"終了しますか？",
			[this]() {
				GameStateClass::SetGameState(GameState::GameEnd);
			});
		});

}

PauseMenu::~PauseMenu()
{
	InputSystem::SetGameMouseMode(InputSystem::RelativeMouse);
	GameStateClass::SetGameState(GameState::GamePlay);
	Time::gTimeScale = 1;
}

void PauseMenu::ProcessInput(const InputState& keys)
{
	Canvas::ProcessInput(keys);
	// ここで入力を処理
	if(keys.Keyboard.GetKeyDown(KEY_TAB))
	{
		Close();
	}
}