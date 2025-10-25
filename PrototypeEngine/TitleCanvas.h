#pragma once
#include "Canvas.h"

//ƒGƒ“ƒWƒ“‚Å‚Í”h¶ƒNƒ‰ƒX‚Æ‚µ‚ÄŠg’£«‚ª‚È‚­‚È‚é‚Ì”p~—\’è

//‘O•ûéŒ¾
class GameDialogBox;

class TitleCanvas : public Canvas
{
protected:
	Image*									mTitleLogo;
	GameDialogBox*							mDialogBox;
public:
											TitleCanvas();
											~TitleCanvas();

	void									Update(float deltaTime) override;
};

