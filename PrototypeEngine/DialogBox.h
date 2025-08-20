#pragma once
#include "Canvas.h"
#include "Image.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//ダイアログボックスUIのクラス
//主にゲーム内の決定を確定するかの処理に使うもの
//生成を行って使用する形で考えている。
class GameDialogBox : public Canvas
{
private:

	std::function<void()> mExitRun;

	std::function<void()> mOnNoRun;
public:
	// (下に引く順序は後ろに対応します)
	GameDialogBox(const string& text,
		std::function<void()> onOK);
	GameDialogBox(const char8_t* text,
		std::function<void()> onOK);
	GameDialogBox(const char8_t* text,
		std::function<void()> onOK,
		std::function<void()> run);
	GameDialogBox(const char8_t* text,
		std::function<void()> onOK,
		std::function<void()> run,
		std::function<void()> exitRun);
	GameDialogBox(const char8_t* text,
		std::function<void()> onOK,
		std::function<void()> onNO,
		std::function<void()> exitRun,
		const bool onOffFrag);
	~GameDialogBox();


	void					Close()override;
};