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

bool ImGuiHelper::IsAncestorOf(ActorObject* potentialAncestor, ActorObject* target)
{
	if (!target || !potentialAncestor)return false;
	if (target == potentialAncestor)return true;

	//target‚Ìe‚ðã‚É‰ˆ‚Á‚Ä‚¢‚­
	ActorObject* current = target->GetTransform()->GetParentActor();
	while (current != nullptr)
	{
		if (current == potentialAncestor)return true;
		current = current->GetTransform()->GetParentActor();
	}

	return false;
}
