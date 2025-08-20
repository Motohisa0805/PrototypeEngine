#pragma once
#include "Canvas.h"
#include "DialogBox.h"

class TitleCanvas : public Canvas
{
protected:
	class Image*							mTitleLogo;
	class GameDialogBox*					mDialogBox;
public:
											TitleCanvas();
											~TitleCanvas();

	void									Update(float deltaTime) override;
};

