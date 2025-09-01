#pragma once
#include "Texture.h"
#include "Renderer.h"

class GUIPanel
{
protected:
	bool isMouseHovered;
private:

public:
	GUIPanel();

	bool IsMouseHovered() const { return isMouseHovered; }
	virtual const char* GetName() { return "BasePanel"; }
};

