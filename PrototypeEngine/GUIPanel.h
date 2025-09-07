#pragma once
#include "Texture.h"
#include "Renderer.h"

//前方宣言
class Renderer;
//GUIのパネルの継承元クラス
//名前、マウスカーソルがパネル内にあるかどうかなどの処理を行っています。
class GUIPanel
{
protected:
	Renderer*			mRenderer;
	bool				isMouseHovered;
private:

public:
	GUIPanel(Renderer* renderer);

	virtual bool		MouseHoveredDisble();

	bool				WindowHoveredConfirmation();

	bool				IsMouseHovered() const { return isMouseHovered; }
	virtual const char* GetName() { return "BasePanel"; }
};

