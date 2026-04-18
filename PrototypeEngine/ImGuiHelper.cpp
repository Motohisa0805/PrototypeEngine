#include "ImGuiHelper.h"

void ImGuiHelper::FragTextButton(const char* lable, const ImVec2& size, bool& frag)
{
	string flag = "Off";
	if (frag)
	{
		flag = "On";
	}
	string buttonText = lable + flag;
	if (ImGui::Button(buttonText.c_str(), ImVec2(0.0f, 0.0f)))
	{
		frag = !frag;
	}
}
