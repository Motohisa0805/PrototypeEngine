#pragma once
#include "Texture.h"
#include "Renderer.h"

//GUIのパネルの継承元クラス
//名前、マウスカーソルがパネル内にあるかどうかなどの処理を行っています。
class GUIPanel
{
protected:
	bool isMouseHovered;
private:

public:
	GUIPanel();

	virtual bool		MouseHoveredDisble();

	bool				WindowHoveredConfirmation();

	bool				IsMouseHovered() const { return isMouseHovered; }
	virtual const char* GetName() { return "BasePanel"; }
};

