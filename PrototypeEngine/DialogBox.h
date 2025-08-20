#pragma once
#include "Canvas.h"
#include "Image.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//�_�C�A���O�{�b�N�XUI�̃N���X
//��ɃQ�[�����̌�����m�肷�邩�̏����Ɏg������
//�������s���Ďg�p����`�ōl���Ă���B
class GameDialogBox : public Canvas
{
private:

	std::function<void()> mExitRun;

	std::function<void()> mOnNoRun;
public:
	// (���Ɉ��������͌��ɑΉ����܂�)
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