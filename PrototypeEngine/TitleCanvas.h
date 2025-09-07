#pragma once
#include "Canvas.h"
#include "DialogBox.h"

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

