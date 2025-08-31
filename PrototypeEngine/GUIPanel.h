#pragma once
#include "Texture.h"
#include "Renderer.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

class GUIPanel
{
private:
	
public:

	virtual const char* GetName() { return "BasePanel"; }
};

